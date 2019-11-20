#!/bin/bash
#
# xbmtool
# create raw graphics data from XBM files for Nokia 5110 LCDs
#
#
# Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
# Released under GPLv2
#
# see `usage` function for general usage information.
#


# enable/disable debug mode
# set to 1 to keep build dir and individually generated files in it
# set to 0 to delete build dir after operations are done
debug=0


xbmtool=$(basename $0)

#
# Print usage and nothing more
#
function usage {
    cat << EOL
Usage: $xbmtool [-f|-l|-a|-g] [-n <namespace>] [-o <outdir>] <xbm files>

Create raw data for Nokia 3310/5110 LCD from given XBM files.
Data can be either created as set of individual graphics, or as an
animation. The animation can be either a regular, one-shot animation
that stops after the last frame, or a looping animation that adds an
extra transition from the last back to the first frame.

Operation mode options:
    -f  Creates a set of individual full frame graphics from given files
    -l  Creates a looping animation from given files
    -a  Creates a one-shot animation from given files
    -g  Creates a set of individual graphics from given files as either
        full frame array, or key diff struct, whichever is more efficient.

Other options:
    -n <namespace>  Optionally set the name space of the generated data.
                    The namespace defines the names of the .c and .h output
                    files, and each generated graphic data array / struct
                    variable name will be pre-fixed with it.
                    If none given, "xbmlib_gfx" is chosen, resulting in
                    xbmlib_gfx.c and xbmlib_gfx.h output files.

    -o <outdir>     Optionally set the output directory for the generated
                    files. If omitted, the directory the script was called
                    from is used instead.

XBM input files:
    All other parameters are expected to be the XBM input files to process.
    The files are processed in the given order, so if creating an animation,
    the anitmation itself is built from that same order. When creation sets
    of individual graphics, the order doesn't really matter.

EOL
}

#
# Set up default values for cli arguments
#
mode_graphics=0
full_graphics=0
mode_animation=0
loop_animation=0
namespace="xbmlib_gfx"
outdir="$(pwd)"

#
# Parse and assign cli arguments accordingly
#
while getopts "lafghn:o:" arg; do
    case $arg in
        l)
            mode_animation=1
            loop_animation=1
            output_type="loop animation"
            ;;
        a)
            mode_animation=1
            loop_animation=0
            output_type="one-shot animation"
            ;;
        f)
            mode_graphics=1
            full_graphics=1
            output_type="full graphics set"
            ;;
        g)
            mode_graphics=1
            full_graphics=0
            output_type="mixed graphics set"
            ;;
        h)
            usage
            exit 0
            ;;
		n)
            namespace="$OPTARG"
            ;;
        o)
            outdir="$OPTARG"
            ;;
        *)
            usage
            exit 1
            ;;
    esac
done

# adjust argument counter
shift $(($OPTIND - 1))

# make sure we have an option selected
if [ $mode_animation -eq 0 ] && [ $mode_graphics -eq 0 ] ; then
    echo "$xbmtool: no mode chosen"
    usage
    exit 1
fi

# make sure there are still enough parameters, i.e. the xbm files
if [ $mode_animation -eq 1 ] && [ $# -lt 2 ] ; then
    echo "$xbmtool: need at least two .xbm files for animations"
    exit 1
elif [ $# -lt 1 ] ; then
    echo "$xbmtool: need at least one .xbm file"
    exit 1
fi

# check if all given files actually exist
files_okay=1
for arg ; do
    if [ ! -f $arg ] ; then
        echo "ERROR: no such file $arg"
        files_okay=0
    fi
done

# nope, at least one of the given files doesn't exist, abort
if [ $files_okay -ne 1 ] ; then
    exit 1
fi

# check that given output directory doesn't exist as file
if [ -e $outdir ] && [ ! -d $outdir ] ; then
    echo "ERROR: $outdir exists but isn't a directory"
    echo "Please choose a different output directory"
    exit 1
fi

# check if outdir exists, create it if not
if [ ! -e $outdir ] ; then
    if [ $debug -eq 1 ] ; then
        echo "Creating output dir $outdir"
    fi
    mkdir -p $outdir || exit 1
elif [ $debug -eq 1 ] ; then
    echo "Using existing output dir $outdir"
fi

outdir_source_file="$outdir/$namespace.c"
outdir_header_file="$outdir/$namespace.h"

# check if generated output files already exist
if [ -e $outdir_source_file ] || [ -e $outdir_header_file ] ; then
    read -p "Output file exists, overwrite? [y/N] " choice
    if [ "${choice,,}" == "y" ] ; then
        echo "Deleting $outdir_source_file and $outdir_header_file"
        rm -f $outdir_source_file $outdir_header_file
    else
        echo "Moving on with existing files"
        exit 0
    fi
fi


# split paramters into array
files=("$@")
file_count=$#

# create build dir
builddir="$(mktemp -d)"
if [ $debug -eq 1 ] ; then
    echo "Using build dir $builddir"
fi

# path to xbmlib directory
srcdir="$(dirname $0)"

xbmgen_header="xbmgen.h"
xbmgen_src="xbmgen.c"
cp $srcdir/$xbmgen_src $builddir

# define output files
header_output_file="$builddir/$namespace.h"
source_output_file="$builddir/$namespace.c"

# storing type and variable name of all animation frames
animation_frame_list=""

# set up gcc flags
gcc_flags=""
gcc_extra_flags=""
if [ $debug -eq 1 ] ; then
    # and some more if debug is enabled
    gcc_flags+="-g -Wall -Wextra"
fi

xbmlib_header_include='#include "xbmlib.h"'
if [ $mode_graphics -eq 1 ] && [ $full_graphics -eq 1 ] ; then
    # no need to include xbmlib.h for full graphics, so let's not
    xbmlib_header_include=''
fi

# write file header to .h output file
cat > $header_output_file << EOL
/*
 * $namespace $output_type
 * auto-generated by $xbmtool
 */
#ifndef ${namespace^^}_H
#define ${namespace^^}_H
#include <stdint.h>
$xbmlib_header_include

EOL

# write file header to .c output file
cat > $source_output_file << EOL
/*
 * $namespace $output_type
 * auto-generated by $xbmtool
 */
#include <avr/pgmspace.h>
#include <stdint.h>
#include "$(basename $header_output_file)"

EOL


#
# Create a graphics frame for either full frame graphics,
# mixed frame graphics or animation key frames.
#
# Parameters:
#   $1  XBM input file path
#   $2  xbmgen.h template path
#
# Returns:
#   Return value from xbmgen and therefore the generate frame type
#
function create_graphic_frame {
    file="$1"
    template="$2"

    prefix="$(basename $file .xbm)"
    outfile="full_frame_${prefix}"

    cp $file $builddir

    sed s/@frame_one@/$prefix/g $template |
    sed s/@namespace@/$namespace/g > $builddir/$xbmgen_header

    if [ $debug -eq 1 ] ; then
        cp $builddir/$xbmgen_header $builddir/${xbmgen_header}-${outfile}
    fi

    if [ $mode_animation -eq 1 ] || [ $full_graphics -eq 0 ] ; then
        gcc_extra_flags="-DMIXED_GRAPHICS"
    fi

    gcc $gcc_flags $gcc_extra_flags -o $builddir/$outfile $builddir/$xbmgen_src

    if [ $mode_animation -eq 1 ] || [ $full_graphics -eq 0 ] ; then
        gcc_extra_flags=""
    fi

    $builddir/$outfile 2>>$header_output_file >>$source_output_file;
    return $?
}

#
# Create a single graphic for either full frame or mixed frame mode.
#
# Parameters:
#   $1  XBM input file path
#
# Returns:
#   Nothing
#
function create_graphic {
    echo "Creating graphic from $1"
    if [ -e $builddir/$1 ] ; then
        echo "WARNING: file is duplicate, skipping"
    else
        create_graphic_frame $1 "$srcdir/xbmgen_h_full.template"
    fi
}

#
# Create a single graphic for the animation key (first) frame
#
# Parameters:
#   $1  XBM input file path
#
# Returns:
#   Nothing
#
function create_key_frame {
    echo "Creating keyframe from $1"
    create_graphic_frame $1 "$srcdir/xbmgen_h_full.template"
    add_frame_to_list $?
}

#
# Create a diff transition for an animation
#
# Parameters:
#   $1  path to XBM file to transition from
#   $2  path to XBM file to transition to
#
# Returns:
#   Return value from xbmgen and therefore the generate frame type
#
function create_diff_frame {
    one="$1"
    two="$2"

    template="$srcdir/xbmgen_h_diff.template"

    prefix_one="$(basename $one .xbm)"
    prefix_two="$(basename $two .xbm)"
    outfile="diff_frame_${prefix_one}-${prefix_two}"

    echo "Creating frame $one -> $two"
    if [ ! -e $builddir/$one ] ; then
        cp $one $builddir
    fi

    if [ ! -e $builddir/$two ] ; then
        cp $two $builddir
    fi

    sed s/@frame_one@/$prefix_one/g $template |
    sed s/@frame_two@/$prefix_two/g |
    sed s/@namespace@/$namespace/g > $builddir/$xbmgen_header

    if [ $debug -eq 1 ] ; then
        cp $builddir/$xbmgen_header $builddir/${xbmgen_header}-${outfile}
    fi

    gcc $gcc_flags $gcc_extra_flags -o $builddir/$outfile $builddir/$xbmgen_src

    $builddir/$outfile 2>>$header_output_file >>$source_output_file;
    return $?
}

#
# Adds animation frame information to the $animation_frame_list based
# on the given frame type that is returned when executing xbmgen.
#
# Parameters:
#   $1  frame type given as return value from xbmgen
#
# Returns:
#   Nothing
#
function add_frame_to_list {
    case $1 in
        0)
            frame_type="TYPE_FULL" ;;
        1)
            frame_type="TYPE_DIFF" ;;
        2)
            frame_type="TYPE_KEY_DIFF" ;;
        *)
            # ignore TYPE_NONE frame type, it won't be added
            return ;;
    esac

    framename=$(sed -n 's/#define framename "\(.*\)"/\1/p' $builddir/$xbmgen_header)
    animation_frame_list+="    {$frame_type, &$framename},\n"
}

#
# Add the content of $animation_frame_list to the generated .h output file
#
# Parameters:
#   None
#
# Returns:
#   Nothing
#
function add_frames_list {
    # remove comma and newline from the last entry (i.e. last three chars)
    animation_frame_list=${animation_frame_list:0:-3}

    cat >> $header_output_file << EOL

/* $output_type frame mapping */
extern const struct xbmlib_frame ${namespace}_frames[];
/* $output_type number of frames */
extern const uint8_t ${namespace}_frames_count;
EOL

    cat >> $source_output_file << EOL

/* $output_type frame mapping */
const struct xbmlib_frame ${namespace}_frames[] PROGMEM = {
$(echo -e "$animation_frame_list")
};

/* $output_type number of frames */
const uint8_t ${namespace}_frames_count = $(echo -e "$animation_frame_list" | wc -l);

EOL
}

#
# Entry point for looping and one-shot animation operation modes
#
# Parameters:
#   None
#
# Returns:
#   Nothing
#
function create_animation_frames {
    # create keyframe data
    create_key_frame ${files[0]}

    # create frame transition diff data
    let frame=0
    while [ $frame -lt $((file_count - 1)) ] ; do
        create_diff_frame ${files[frame]} ${files[$((frame + 1))]}
        add_frame_to_list $?
        let frame=frame+1
    done

    if [ $loop_animation -eq 1 ] ; then
        gcc_extra_flags="-DLAST_DIFF_FRAME"
        create_diff_frame ${files[frame]} ${files[0]}
        add_frame_to_list $?
        gcc_extra_flags=""
    fi
}

#
# Entry point for full and mixed frame graphics set operation mode
#
# Parameters:
#   None
#
# Returns:
#   Nothing
#
function create_graphics {
    for file in ${files[@]} ; do
        create_graphic $file
    done
}

#
# Start the processing
#
if [ $mode_animation -eq 1 ] ; then
    create_animation_frames
    add_frames_list
else
    create_graphics
fi

# write file footer to .h output file
cat >> $header_output_file << EOL

#endif
EOL

# copy generated header and source file in current directory
echo "Writing $output_type to $outdir_source_file and $outdir_header_file"
cp $header_output_file $outdir
cp $source_output_file $outdir

# clean up if needed
if [ $debug -ne 1 ] ; then
    rm -rf $builddir
fi

# aaand we're done.
exit 0

