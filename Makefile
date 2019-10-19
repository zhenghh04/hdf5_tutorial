#Makefile
#!/bin/sh
CC=mpicc
CXX=mpicxx
CFLAGS=-O3
LIBS=-L$(HDF5_ROOT)/lib -lhdf5

dset:
	$(CXX) $(CFLAGS) -o create_dset.x create_dset.cpp $(LIBS)
	./create_dset.x
	h5dump ./file.h5 

clean:
	rm -rf *.x
