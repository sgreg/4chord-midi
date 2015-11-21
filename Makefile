all: tools firmware

tools:
	make -C tools/

firmware:
	make -C firmware/

clean:
	make -C tools/ clean
	make -C firmware/ clean

distclean: clean
	make -C firmware/ distclean

.PHONY : all tools firmware clean distclean

