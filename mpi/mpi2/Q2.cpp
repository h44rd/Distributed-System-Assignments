#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <mpi.h>
using namespace std;

int N;
char ** adj; // adjacency matrix
char * adj_matrix; // actual store
bool ** colors; // color classes
bool ** free_color; // free color classes of nodes
int * sat; // saturation of a node
int * free_num; // number of free color classes for nodes
bool * OK; // is the node ready
int num_color = 0;

void initialize() {
  int sum;
  for (int i = 0; i < N; i++) {
    sum = 0;
    for (int j = 0; j < N; j++) {
      colors[i][j] = 0;
      free_color[i][j] = 0;
      sum += adj[i][j];
    }
    sat[i] = sum;
    free_num[i] = 0;
    OK[i] = 0;
  }
}

void memory_init() {
  int i, j;
  adj = new char * [N];
  adj_matrix = new char[N * N];
  colors = new bool * [N];
  free_color = new bool * [N];

  for (i = 0; i < N; i++) {
    adj[i] = adj_matrix + (i * N);
    colors[i] = new bool[N];
    free_color[i] = new bool[N];
  }

  sat = new int[N];
  free_num = new int[N];
  OK = new bool[N];

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      adj[i][j] = 0;
}

void read_graph() {
  int i, j, n, m, x, y;
  string type, line;

  cin >> N; // vertexes
  cin >> m; // edges
  // cout<<"Number of nodes: "<<N<<" Number of edges: "<<m;
  //allocate apropriate space according to N:
  memory_init();

  for (i = 0; i < m; i++) {
    cin >> x;
    cin >> y;
    adj[x][y] = 1;
    adj[y][x] = 1;
  }
  // cout<<"Done with taking the input\n";
}

void display_output() {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < num_color; j++) {
      if (colors[j][i]) {
        cout << j << endl;
      }
    }
  }
}

int main(int argc, char ** argv) {
  int i, j, c, min_free, min_sat, min_id;
  int id, nproc, id_from;
  int startval, endval, sv, ev;

  MPI_Status status;
  MPI_Init( & argc, & argv);
  MPI_Comm_rank(MPI_COMM_WORLD, & id);
  MPI_Comm_size(MPI_COMM_WORLD, & nproc);

  if (id == 0) {
    read_graph();
  }

  MPI_Bcast( & N, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (id != 0) {
    memory_init();
  }

  startval = N * id / nproc;
  endval = N * (id + 1) / nproc;

  MPI_Bcast(adj_matrix, N * N, MPI_CHAR, 0, MPI_COMM_WORLD);

  initialize();

  for (int v = 0; v < N; v++) {
    min_free = N;
    min_sat = N;
    min_id = -1;

    for (i = startval; i < endval; i++)
      if (!OK[i] && (min_free > free_num[i] ||
          min_free == free_num[i] && sat[i] < min_sat)) {
        min_free = free_num[i];
        min_sat = sat[i];
        min_id = i;
      }

    struct {
      int free;
      int id;
    }
    p, tmp_p;
    p.id = min_id;
    p.free = min_free;
    MPI_Allreduce( & p, & tmp_p, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

    min_id = tmp_p.id;
    min_free = tmp_p.free;

    OK[min_id] = 1;

    if (min_free == 0) {
      colors[num_color][min_id] = 1;

      for (i = startval; i < endval; i++)
        if (!OK[i]) {
          free_color[i][num_color] = 1;
          ++free_num[i];
        }

      for (i = startval; i < endval; i++) {
        if (!OK[i] && adj[i][min_id]) {
          free_color[i][num_color] = 0;
          free_num[i]--;
        }
      }
      num_color++;
    } else {
      //We put node into an old color class.
      //find the class:
      int id_from;

      for (id_from = 0; id_from < nproc; id_from++)
        if (N * id_from / nproc <= min_id && min_id < N * (id_from + 1) / nproc) break;

      if (id == id_from)
        for (c = 0; !free_color[min_id][c]; c++);

      MPI_Bcast( & c, 1, MPI_INT, id_from, MPI_COMM_WORLD);

      colors[c][min_id] = 1;

      for (i = startval; i < endval; i++) {
        if (!OK[i] && free_color[i][c] && adj[i][min_id]) {
          free_color[i][c] = 0;
          free_num[i]--;
        }
      }

    }
  }
  if (id == 0) {
    cout << num_color << endl;
    display_output();
  }
  MPI_Finalize();

}
