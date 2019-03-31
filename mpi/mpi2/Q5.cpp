#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
// vector <int> forward;
// vector <int> back;
int fwd_arr[100][1000];
int back_arr[100][1000];
int pairs [2][100];
int fsize[100]; int bsize[100];

long get_com (int a[], int b[], int at, int bt){
  int ap = 0; int bp = 0;
  long sum = 0;
  // for(int i=0; i<at;i ++){
  //   cout << a[i] << " ";
  // }
  while (ap < at and bp < bt){
    if(a[ap] == b[bp]){
      sum ++;
      // cout << "yesihvihegh" << endl;
      ap ++; bp ++;
    }
    else if (a[ap] > b[bp]) bp ++;
    else ap ++;
  }
  // cout << "sum " << sum << endl;
  return sum;
}
void send_int (int n, int to) {
  MPI_Send(&n, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}

int recv_int (int from) {
  int n;
  MPI_Recv(&n, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  return n;
}
void send_long (long n, int to) {
  MPI_Send(&n, 1, MPI_LONG, to, 0, MPI_COMM_WORLD);
}

long recv_long (int from) {
  long n;
  MPI_Recv(&n, 1, MPI_LONG, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  return n;
}

void send_arr(int* arr, int len, int to) {
  send_int(len, to);
  MPI_Send(arr, len, MPI_INT, to, 0, MPI_COMM_WORLD);
}

vector<int> recv_arr (int from) {
  int len = recv_int(from);
  int arr[len];
  MPI_Recv(arr, len, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  return vector<int>(arr, arr + len);
}


int main (int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int n_proc = world_size - 1;
  int n, e, len;
  if (rank == 0) {
      cin >> n >> e;
    int u,v;
    for (int i = 0; i < e; i++) {
      cin >> u >> v;
      fwd_arr[u][fsize[u]] = v;
      fsize[u] ++;
      back_arr[v][bsize[v]] = u;
      bsize[v] ++;
    }
    for (int i = 0; i < n; i++){
      sort(&fwd_arr[i][0], &fwd_arr[i][0] + fsize[i]);
      sort(&back_arr[i][0], &back_arr[i][0] + bsize[i]);
    }
    int lim;
    // for (int i = 0; i < n; i++){
    //   cout << "FWD" <<i << endl;
    //     for(int j=0; j< fsize[i]; j++){
    //       cout << fwd_arr[i][j] << " ";
    //     }
    //   }
    //   cout << endl;
    //   for (int i = 0; i < n; i++){
    //     cout << "BCK" <<i << endl;
    //       for(int j=0; j< bsize[i]; j++){
    //         cout << back_arr[i][j] << " ";
    //       }
    //     }
    //     cout << endl;

  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&fsize, n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&bsize, n,  MPI_INT, 0, MPI_COMM_WORLD);
  for (int i = 0; i < n; i++) {
    MPI_Bcast(&fwd_arr[i], 1000, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&back_arr[i], 1000, MPI_INT, 0, MPI_COMM_WORLD);
  }

  long start, end;
  if (rank != 0) {
    int count = 0;
    for (int i = 0; i < n - 1; i++){
      for (int j = i + 1; j < n; j++){
          pairs[0][count] = i;
          pairs[1][count] = j;
            // cout << pairs[0][count] << " " << pairs[1][count] << endl;
          count ++;
      }
    }
    long tot = n * (n - 1) / 2;
    start = (tot / n_proc) * (rank - 1);
    end = (tot / n_proc) * rank;
  }

  if (rank == n_proc) {
    end = n * (n - 1) / 2;
  }

  if (rank != 0) {
      long com = 0;
    for (int i= start; i <= end; i++){
      com += get_com(fwd_arr[pairs[0][i]], back_arr[pairs[1][i]], fsize[pairs[0][i]], bsize[pairs[1][i]]);
      com += get_com(fwd_arr[pairs[1][i]], back_arr[pairs[0][i]], fsize[pairs[1][i]], bsize[pairs[0][i]]);
    }
    long tmp = com*(com-1)/2;
    send_long(tmp, 0);
  }

  if (rank == 0) {
    long sum = 0;
    // cout << "HERE" << rank << endl;
    for (int i = 0; i < n_proc; i++) {
      sum += recv_long(i + 1);
      // cout << sum << endl;
    }
    long ans = sum /2;
    cout << ans << endl;
  }

  // Finalize the MPI environment.
  MPI_Finalize();

  return 0;
}
