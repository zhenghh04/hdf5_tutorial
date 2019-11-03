#Makefile
#!/bin/sh
CC=mpicc
CXX=mpicxx
CFLAGS=-O3 -DDEBUG
LIBS=-Wl,-rpath,$(HDF5_ROOT)/lib -L$(HDF5_ROOT)/lib -lhdf5 

parallel_hdf5:
	$(CXX) $(CFLAGS) -o parallel_hdf5.x parallel_hdf5.cpp $(LIBS)

dset:
	$(CXX) $(CFLAGS) -o create_dset.x create_dset.cpp $(LIBS)
	./create_dset.x
	h5dump ./file.h5 
clean:
	rm -rf *.x *.h5 *.hdf5
