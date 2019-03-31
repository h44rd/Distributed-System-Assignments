#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;


int main(int argc, char const *argv[])
{
	int id,size,N,u,v,w,mini,min_vertex,E;
	int adjMat[50][50];
	int num_vertices;
	int main_mst_status[50];

	MPI_Init(NULL, NULL);
	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	//cout << size << id << "\n";

	if(id == 0)
	{
		cin >> N >> E;
		for(int i=0; i<N; i++)
		{
			for(int j=0; j<E; j++)
			{
				if(i==j)
					adjMat[i][j] = 0;
				else
					adjMat[i][j] = -1;
				main_mst_status[i] = 0;
			}
		}

		for(int i=0; i<E; i++)
		{
			cin >> u >> v >> w;
			adjMat[u-1][v-1] = w;
			adjMat[v-1][u-1] = w;
		}

		num_vertices = (N)/(size-1);
	}


	MPI_Bcast (&num_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if(id == 0)
		{


			for(int j=0; j<N; j++)
			{
				MPI_Send(&j, 1, MPI_INT, (j%(size-1))+1, 1, MPI_COMM_WORLD);
				MPI_Send(&adjMat[j], N, MPI_INT, (j%(size-1))+1, 1, MPI_COMM_WORLD);
			}

			int vertex = 1;
			for(int i=0; i<N-1; i++)
			{
				//vertex is the global minimum vertex
				// cout << "vertex = " << vertex << endl;
				mini = 100000;
				int process_min[2];
				main_mst_status[vertex] = 1;
				for(int j=1; j<=(size-1); j++)
				{
					MPI_Send(&vertex, 1, MPI_INT, j, 1, MPI_COMM_WORLD);
					MPI_Recv(&process_min, 2, MPI_INT, j, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					if((process_min[0] != -1)&&(process_min[1] < mini))
					{
						mini = process_min[1];
						min_vertex = process_min[0];
					}
				}
				vertex = min_vertex;
				cout << "vertex = " << vertex << ", weight = "<< mini <<endl;
			}
		}
		else
		{
			int adjmat[N],u;
			map <int, vector <int> > adj;
			map <int, int> mst_status;
			map <int, int> dist;
			int vertex;
			int answer[2];
			for(int i=0; i<num_vertices; i++)
			{
				MPI_Recv(&vertex, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&adjmat, N, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				vector <int> v;
				// cout << "id = "<<id << ", vertex = " << vertex << endl;
				for (int i = 0; i < N; i++)
				{
					v.push_back(adjmat[i]);
					cout << adjmat[i] <<" ";
				}
				cout << endl;
				adj[vertex] = v;
				mst_status[vertex] = 0;
				dist[vertex] = 100000;
			}

			for(int i=0; i<N-1; i++)
			{
				answer[0] = -1;
				answer[1] = -1;
				int min = 1000000;
				MPI_Recv(&u, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				map<int,int>::iterator it;
				it = mst_status.find(u);
				if(it != mst_status.end())
				{
					mst_status[u] = 1;
				}
				for(it = dist.begin(); it != dist.end(); it++)
				{
					int ver = it->first;
					if(ver != u)
					{
						if(mst_status[ver] == 0)
						{
							if((adj[ver][u] >0 )&& (adj[ver][u] < dist[ver]))
							{
								dist[ver] = adj[ver][u];
								// if(dist[ver] < min)
								// {
								// 	min = dist[ver];
								// 	answer[0] = ver;
								// 	answer[1] = min;
								// }
							}
						}
					}
				}
				for(it = dist.begin(); it != dist.end(); it++)
				{
					int ver = it->first;
					if(mst_status[ver]==0 && dist[ver] < min)
					{
						min = dist[ver];
						answer[0] = ver;
						answer[1] = min;
					}
				}
				//cout << "vertex = "<< u << ",answer[0] = "<< answer[0] << ", answer[1] = "<<answer[1] << ", rank = "<< id << endl;
				MPI_Send(&answer, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
			}
		}

	// }
	MPI_Finalize();
	return 0;
}
