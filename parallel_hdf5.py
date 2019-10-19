import h5py
from mpi4py import MPI

comm = MPI.COMM_WORLD
print("I am rank %s of %s"%(comm.rank, comm.size))
if (comm.rank==0):
    print("h5py version: ", h5py.__version__)
d1 = 8
d2 = 5
f = h5py.File("parallel_file.hdf5", 'w', driver='mpio', comm = comm)

dset = f.create_dataset("dset", (d1, d2), dtype='i')
dset.attrs['Units'] = "Images per second"
for i in range(comm.rank, d1, comm.size):
    for j in range(d2):
        dset[i, j] = i*d2 + j + 1
f.close()

