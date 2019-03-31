#include <iostream>
#include <mpi.h>
#include <math.h>
#include<utility>
#include<algorithm>

using namespace std;


#define send_data_tag 0

const double PI = acos(-1);
const double eps = 1e-9;

double get_angle(double x, double y)
{
    if(fabs(x) < eps)
        return PI/2;

    if (x > 0)
    {
        if(y > 0)
            return atan(y/x);

        return 2*PI + atan(y/x);
    }

    return PI + atan(y/x);
}


bool compare(pair <double, double> a, pair <int, int> b)
{
    double th1 = get_angle(a.first, a.second);
    double th2 = get_angle(b.first, b.second);

    return th1 < th2;
}

int main(int argc, char **argv)
{
    int ierr;

    double X = 0, Y = 0;
    // int x[100], y[100];
    int p[2];
    pair <int, int> points[100];


    ierr = MPI_Init(&argc, &argv);

    int rootp = 0, my_id, num_procs, num_child_procs;
    int per_proc,  start, end, is_divisible, is_final_divisible, sender, flag_con = 1;
    MPI_Status status;

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    num_child_procs = num_procs - 1;

    int n;

    if(my_id == rootp)
    {
        // printf("Please enter the number: ");
        cin>>n;

        for(int i = 0; i < n; i++)
        {
            // cout<<i<<'\n';
            cin>>points[i].first;
            X += points[i].first;

            cin>>points[i].second;
            Y += points[i].second;
        }

        X /= n;
        Y /= n;

        for(int i = 0; i < n; i++)
        {
            points[i].first -= X;
            points[i].second -= Y;
        }

        sort(points, points + n, compare);

        // for(int i = 0; i < n; i++)
        // {
        //     cout<<points[i].first<<" "<<points[i].second<<'\n';
        // }

        int num_tris = 0, id = 0, num_iter = 0;
        float temp_area, total_area = 0;
        while(num_tris < n - 2)
        {
            id = (num_tris)%num_child_procs + 1;

            ierr = MPI_Send( &flag_con, 1 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
            p[0] = points[0].first;
            p[1] = points[0].second;
            ierr = MPI_Send( p, 2 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
            p[0] = points[num_tris+1].first;
            p[1] = points[num_tris+1].second;
            ierr = MPI_Send( p, 2 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
            p[0] = points[num_tris+2].first;
            p[1] = points[num_tris+2].second;
            ierr = MPI_Send( p, 2 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);

            num_tris++;

            // cout<<num_tris<<"----------\n";

            if(num_tris % num_child_procs == 0)
            {
                // cout<<"========================\n";
                for(int id1 = 1; id1 <= num_child_procs; id1++)
                {
                    ierr = MPI_Recv( &temp_area, 1, MPI_FLOAT, id, send_data_tag, MPI_COMM_WORLD, &status);
                    total_area += temp_area;
                }
            }
        }

        for(int id1 = 1; id1 <= (num_tris) % num_child_procs; id1++)
        {
            // cout<<"Outer loop ======="<<id1<<"=====\n";
            ierr = MPI_Recv( &temp_area, 1, MPI_FLOAT, id1, send_data_tag, MPI_COMM_WORLD, &status);
            total_area += temp_area;
        }
        cout<<"Area: "<<total_area<<"\n";
        flag_con = 0;
        for(int j = 1; j <= num_child_procs; j++)
        {
            ierr = MPI_Send(&flag_con, 1, MPI_INT, j, send_data_tag, MPI_COMM_WORLD);
        }
    }
    else
    {
        int p1[2], p2[2], p3[2];
        while (true)
        {
            ierr = MPI_Recv( &flag_con, 1, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);
            if(flag_con == 0)
            {
                exit(0);
            }
            ierr = MPI_Recv( p1, 2, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);
            ierr = MPI_Recv( p2, 2, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);
            ierr = MPI_Recv( p3, 2, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);

            float area = fabs(1.0*(p1[0]*(p2[1] - p3[1]) + p2[0]*(p3[1] - p1[1]) + p3[0]*(p1[1] - p2[1])))/2;
            // cout<<"Process: "<<my_id<<" Area: "<<area<<"\n";
            ierr = MPI_Send( &area, 1 , MPI_FLOAT, rootp, send_data_tag, MPI_COMM_WORLD);
            // cout<<"Process: "<<my_id<<" ended "<<area<<"\n";
        }

    }
    ierr = MPI_Finalize();

}
