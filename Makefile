#Makefile
#!/bin/sh
CC=mpicc
CXX=mpicxx
CFLAGS=-O3
LIBS=-L$(HDF5_ROOT)/lib -lhdf5 -lmpitrace_mpich

dset:
	$(CXX) $(CFLAGS) -o create_dset.x create_dset.cpp $(LIBS)
	./create_dset.x
	h5dump ./file.h5 
parallel_hdf5:
	$(CXX) $(CFLAGS) -o parallel_hdf5.x parallel_hdf5.cpp $(LIBS)
	mpirun -np 4 ./parallel_hdf5.x
	h5dump ./parallel_file.h5 
clean:
	rm -rf *.x *.h5 *.hdf5
