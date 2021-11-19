#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

#define BUFFSIZE 10

void RandomTime(struct timespec *req)
{
    float random = ((float) rand()) / (float) RAND_MAX;

    float randTime = 0.1 + 1.9 * random;
    req->tv_sec = 0;
    if (randTime > 1)
    {
        req->tv_sec = 1;
        randTime -= 1;
    }
    req->tv_nsec == 999999999 * randTime;
}

int main(int argc, char **argv)
{
    struct shm_struct 
    {
        int buffer[BUFFSIZE];
        unsigned count;
    };
    volatile struct shm_struct *shmp = NULL;
    char *addr = NULL;
    pid_t pid = -1;
    int var1 = 0, var2 = 0, shmid = -1;
    struct shmid_ds *shm_buf;
    /*allocate a chunk of shared memory*/
    shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
    shmp = (struct shm_struct*) shmat(shmid, addr, 0);
    shmp->count = 0;
    pid = fork();

    //both parent and child has an index for their current position in buffer and has req and rem to randomize and wait a certian time.
    int index = 0;
    struct timespec req;
    struct timespec rem;

    if(pid != 0) 
    {
        srand(13);
        /*here’s the parent, acting as producer*/
        while(var1 < 100) 
        {
            /*write to shmem*/
            var1++;
            while(shmp->count == BUFFSIZE);
            /*If buffer is full the producer will busy wait until the consumer has removed something from the buffer*/
            //Race condition may occur when both the consumer and producer enter this critical region simultaneously and values may be lost
            printf("Sending %d\n", var1);
            fflush(stdout);
            shmp->buffer[index] = var1;
            shmp->count++;
            
            index++;
            if (index == BUFFSIZE)
            {
                index = 0;
            }
            RandomTime(&req);
            nanosleep(&req, &rem);
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    }
    else
    {
        srand(42);
        /*here’s the child, acting as consumer*/
        while(var2 < 100) 
        {
            /*read from shmem*/
            while(shmp->count == 0);
            /*If buffer is empty the consumer will busy wait until the producer has added something to the buffer*/
            var2 = shmp->buffer[index];
            shmp->count--;
            printf("Received %d\n", var2); 
            fflush(stdout);

            index++;
            if (index == BUFFSIZE)
            {
                index = 0;
            }
            RandomTime(&req);
            nanosleep(&req, &rem);
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    }
}