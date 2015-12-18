TMPDIR=$(mktemp -d)

cp 4chord-midi-B.Cu.gbr $TMPDIR/4chord-midi.GBL
cp 4chord-midi-B.Mask.gbr $TMPDIR/4chord-midi.GBS
cp 4chord-midi-B.SilkS.gbr $TMPDIR/4chord-midi.GBO
cp 4chord-midi.drl $TMPDIR/4chord-midi.XLN
cp 4chord-midi-Edge.Cuts.gbr $TMPDIR/4chord-midi.GKO
cp 4chord-midi-F.Cu.gbr $TMPDIR/4chord-midi.GTL
cp 4chord-midi-F.Mask.gbr $TMPDIR/4chord-midi.GTS
cp 4chord-midi-F.SilkS.gbr $TMPDIR/4chord-midi.GTO

cd $TMPDIR
zip 4chord-midi.zip *
cd -
cp $TMPDIR/4chord-midi.zip .

rm -rf $TMPDIR

