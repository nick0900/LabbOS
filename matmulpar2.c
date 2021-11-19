/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

void *Init(void *param)
{
    int *row = (int *)param;

    for (int j = 0; j < SIZE; j++)
    {
        /* Simple initialization, which enables us to easy check
            * the correct answer. Each element in c will have the same
            * value as SIZE after the matmul operation.
            */
        a[*row][j] = 1.0;
        b[*row][j] = 1.0;
    }

    return NULL;
}

static void
init_matrix(pthread_t * threads)
{
    int i, j;
    int threadData[SIZE];

    for (i = 0; i < SIZE; i++)
    {
        threadData[i] = i;
        pthread_create(&(threads[i]), NULL, Init, &threadData[i]);
    }
        
}

void *Work(void *param)
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
        pthread_create(&(threads[i]), NULL, Work, &threadData[i]);
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
    pthread_t threads[SIZE];

    init_matrix(threads);

    for (int i = 0; i < SIZE; i++)
    {
        pthread_join(threads[i], NULL);
    }

    matmul_para(threads);

    for (int i = 0; i < SIZE; i++)
    {
        pthread_join(threads[i], NULL);
    }
    //print_matrix();
}
