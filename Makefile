all: libfmipp simple event simple-inc event-inc
libfmipp:
	make -f makefile.linux clean
	make -f makefile.linux build
event:
	cd examples/fmu/event; make -f makefile.linux clean; make -f makefile.linux build
simple:
	cd examples/fmu/simple; make -f makefile.linux clean; make -f makefile.linux build
simple-inc:
	cd examples/incrementalfmu/simple; make all
event-inc:
	cd examples/incrementalfmu/event; make all
