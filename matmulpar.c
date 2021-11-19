/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024

volatile double a[SIZE][SIZE];
volatile double b[SIZE][SIZE];
volatile double c[SIZE][SIZE];

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
        {
            /* Simple initialization, which enables us to easy check
             * the correct answer. Each element in c will have the same
             * value as SIZE after the matmul operation.
             */
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
}

void *ThreadWork(void *param)
{
    int *row = (int *)param;

    for (int j = 0; j < SIZE; j++)
    {
        c[*row][j] = 0.0;
        for (int k = 0; k < SIZE; k++)
            c[*row][j] = c[*row][j] + a[*row][k] * b[k][j];
    }
    return NULL;
}

static void
matmul_para(pthread_t * threads)
{
    int i;
    int threadData[SIZE];

    for (i = 0; i < SIZE; i++)
    {
        threadData[i] = i;
        pthread_create(&(threads[i]), NULL, ThreadWork, &threadData[i]);
    }
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
            printf(" %7.2f", c[i][j]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    init_matrix();

    pthread_t threads[SIZE];
    matmul_para(threads);

    for (int i = 0; i < SIZE; i++)
    {
        pthread_join(threads[i], NULL);
    }
    //print_matrix();
}
