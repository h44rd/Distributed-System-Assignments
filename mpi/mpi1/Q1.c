#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define send_data_tag 0

int main(int argc, char **argv)
{
    int ierr;

    ierr = MPI_Init(&argc, &argv);

    int rootp = 0, my_id, num_procs, num_child_procs;
    int n, rt_n;
    int per_proc,  start, end, is_divisible, is_final_divisible, sender;
    MPI_Status status;

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    num_child_procs = num_procs - 1;

    if(my_id == rootp)
    {
        // printf("Please enter the number: ");
        scanf("%d", &n);

        rt_n = ceil(sqrt(n)) + 1;

        if(rt_n >= num_child_procs)
        {
            per_proc = rt_n/num_child_procs;
        }
        else
        {
            per_proc = 1;
        }

        for(int id = 1; id <= num_child_procs; id++)
        {
            start = id*per_proc;
            end = (id + 1)*per_proc;

            ierr = MPI_Send( &start, 1 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
            ierr = MPI_Send( &end, 1 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
            ierr = MPI_Send( &n, 1 , MPI_INT, id, send_data_tag, MPI_COMM_WORLD);
        }

        is_final_divisible = 0;
        for(int id = 1; id <= num_child_procs; id++)
        {
            ierr = MPI_Recv( &is_divisible, 1, MPI_INT, id, send_data_tag, MPI_COMM_WORLD, &status);
            // printf("Divisibility from process %d : %d\n", id, is_divisible);
            if(is_divisible == 1)
            {
                is_final_divisible = 1;
            }
        }
        if(is_final_divisible == 0)
        {
            printf("%d is a prime\n", n);
        }
        else
        {
            printf("%d is not a prime\n", n);
        }
    }
    else
    {
        ierr = MPI_Recv( &start, 1, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);
        ierr = MPI_Recv( &end, 1, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);
        ierr = MPI_Recv( &n, 1, MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD, &status);

        is_divisible = 0;
        for(int i = start; i < end; i++)
        {
            if(n % i == 0 && i != 1)
            {
                is_divisible = 1;
                break;
            }
        }

        ierr = MPI_Send( &is_divisible, 1 , MPI_INT, rootp, send_data_tag, MPI_COMM_WORLD);
    }
    ierr = MPI_Finalize();
}
