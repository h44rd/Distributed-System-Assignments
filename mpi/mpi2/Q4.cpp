#include<iostream>
// #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
#include<mpi.h>
// #include<math.h>
using namespace std;

void printmat(int a[][100], int n)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
            cout << a[i][j] << " ";
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    int rank;
    int size;

    if(MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        fprintf(stderr, "Unable to initialize MPI!\n");
        return -1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // cerr << rank << size;
    if(rank == 0)
    {
        int n, e, k;
        int g[100][100], currval[100][100], temp[100][100];
        cin >> n >> e >> k;
        // cout << rank << endl;
        // scanf("%d %d %d", &n, &e, &k);
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                g[i][j] = currval[i][j] = 0;
        for(int i = 0; i < e; i++)
        {
            int x, y;
            // scanf("%d %d", &n, &e, &k);
            cin >> x >> y;
            x--, y--;
            g[x][y] = currval[x][y] = 1;
        }
        // printmat(currval, n);
        for(int i = 1; i < size; i++)
        {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        while(k-- > 1)
        {
            // printmat(currval, n);
            // cout << endl;
            int currproc = 1;
            for(int i = 0; i < n; i++)
            {
                for(int j = 0; j < n; j++)
                {
                    // cout << currval[i][j] << " " << g[i][j] << endl;
                    MPI_Send(g[i], n, MPI_INT, currproc, 0, MPI_COMM_WORLD);
                    int temparr[100], resval;
                    for(int tempiter = 0; tempiter < n; tempiter++)
                        temparr[tempiter] = currval[tempiter][j];
                    MPI_Send(temparr, n, MPI_INT, currproc, 0, MPI_COMM_WORLD);
                    MPI_Recv(&resval, 1, MPI_INT, currproc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    // cout << resval << endl;
                    temp[i][j] = resval;
                    currproc = currproc%(size-1) + 1;
                }
            }
            for(int i = 0; i < n; i++)
                for(int j = 0; j < n; j++)
                    currval[i][j] = temp[i][j];
        }
        // printmat(currval, n);
        int finalans = 0;
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                finalans += currval[i][j];
        cout << finalans << endl;
    }
    else
    {
        // printf("RANK!");
        int n, k, x[100], y[100];
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        while(k--)
        {
            for(int i = 0; i <= n*n/(size-1); i++)
            {
                if(rank > (n*n)%(size-1) && i == n*n/(size-1))
                    break;
                // cout << "RANK_START = " << rank << endl;
                // cout << "RANK_START = " << rank << endl;
                MPI_Recv(x, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(y, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int ans = 0;
                for(int j = 0; j < n; j++)
                    ans += x[j]*y[j];
                MPI_Send(&ans, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
