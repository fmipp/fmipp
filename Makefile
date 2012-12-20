all: libfmipp einfach event einfach-inc event-inc
libfmipp:
	make -f makefile.linux clean
	make -f makefile.linux build
event:
	cd examples/fmu/event; make -f makefile.linux clean; make -f makefile.linux build
einfach:
	cd examples/fmu/einfach; make -f makefile.linux clean; make -f makefile.linux build
einfach-inc:
	cd examples/incrementalfmu/einfach; make all
event-inc:
	cd examples/incrementalfmu/event; make all
