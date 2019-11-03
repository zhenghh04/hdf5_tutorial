#include "hdf5.h"
#include <iostream>
#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"
#include <sys/time.h>
#include <string.h>
using namespace std; 
int main(int argc, char **argv) {
  // Assuming that the dataset is a two dimensional array of 8x5 dimension;
  int d1 = 9; 
  int d2 = 5; 
  int niter = 10; 
  for(int i=1; i<argc; i++) {
    if (strcmp(argv[i], "--dim")==0) {
      d1 = int(atoi(argv[i+1])); 
      d2 = int(atoi(argv[i+2])); 
      i+=3; 
    } else if (strcmp(argv[i], "--niter")==0) {
      niter = int(atoi(argv[i+1])); 
      i+=2; 
    }
  }
  hsize_t gdims[2] = {d1, d2};
  
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Info info = MPI_INFO_NULL;
  int rank, nproc; 
  MPI_Init(&argc, &argv);
  MPI_Comm_size(comm, &nproc);
  MPI_Comm_rank(comm, &rank);
  cout << "I am rank " << rank << " of " << nproc << endl; 
  // find local array dimension and offset; 
  hsize_t ldims[2];
  hsize_t offset[2]; 
  ldims[0] = gdims[0]/nproc;
  ldims[1] = gdims[1];
  offset[0] = rank*ldims[0];
  offset[1] = 0;
  if(gdims[0]%ldims[0]>0)
    if (rank==nproc-1)
      ldims[0] += gdims[0]%ldims[0]; 
  // setup file access property list for mpio
  hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(plist_id, comm, info);
  // Create file



  hid_t file_id = H5Fcreate("parallel_file.h5", H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
  H5Pclose(plist_id);
#ifdef DEBUG
  if (rank==0) cout << "Created file: " << endl; 
#endif
  // create dataspace
  hid_t filespace = H5Screate_simple(2, gdims, NULL);
  hid_t dset_id = H5Dcreate(file_id, "dset", H5T_NATIVE_INT, filespace, H5P_DEFAULT,
			    H5P_DEFAULT, H5P_DEFAULT);
#ifdef DEBUG
  if (rank==0) cout << "Created dataspace: " << endl; 
#endif

  // define local data
  int* data = new int[ldims[0]*ldims[1]];
  for(int i=0; i<ldims[0]*ldims[1]; i++)
    data[i] = rank + 10;

  // set up dataset access property list 
  plist_id = H5Pcreate(H5P_DATASET_XFER);


  H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
  // define local memory space
  hid_t memspace = H5Screate_simple(2, ldims, NULL);
  H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, ldims, NULL);
  // write dataset;
#ifdef DEBUG
  if (rank==0) cout << "H5Dwrite ... " << endl; 
#endif
  double t0 = MPI_Wtime(); 
  for (int i=0; i<niter; i++)
    hid_t status = H5Dwrite(dset_id, H5T_NATIVE_INT, memspace, filespace, plist_id, data);
  double t1 = MPI_Wtime(); 
  if (rank==0) cout <<"time: " << t1 - t0 << endl; 
  H5Pclose(plist_id);
  delete [] data;
  H5Dclose(dset_id);
  H5Sclose(filespace);
  H5Sclose(memspace);
  H5Fclose(file_id);
  MPI_Finalize();
  return 0;
}
