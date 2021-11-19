#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

#define BUFFSIZE 10

const char *semName1 = "sema1";
const char *semName2 = "sema2";
const char *semName3 = "sema3";

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
        sem_t *full;
        sem_t *empty;
        sem_t *bufferLock;
    };
    volatile struct shm_struct *shmp = NULL;
    char *addr = NULL;
    pid_t pid = -1;
    int var1 = 0, var2 = 0, shmid = -1;
    struct shmid_ds *shm_buf;
    /*allocate a chunk of shared memory*/
    shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
    shmp = (struct shm_struct*) shmat(shmid, addr, 0);
    shmp->full = sem_open(semName1, O_CREAT, O_RDWR, 0);
    shmp->empty = sem_open(semName2, O_CREAT, O_RDWR, 10);
    shmp->bufferLock = sem_open(semName3, O_CREAT, O_RDWR, 1);
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
            
            //the producer will decrement the empty slots, if the buffer is full i.e empty = 0 it will wait until the consumer increment the empty slots
            sem_wait(shmp->empty);
            
            //the producer will try and decrement the lock and enter the dritical region, or wait for the consumer to leave the critical region
            sem_wait(shmp->bufferLock);
            printf("Sending %d\n", var1);
            fflush(stdout);
            shmp->buffer[index] = var1;
            sem_post(shmp->bufferLock);
            
            //the producer leaves the critical region and increment the number of full buffer slots;
            sem_post(shmp->full);
            
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
            //the consumer decrement the number of full slots or wait until there are any
            sem_wait(shmp->full);

            //Then it tries to enter the critical region to read
            sem_wait(shmp->bufferLock);
            var2 = shmp->buffer[index];
            printf("Received %d\n", var2); 
            fflush(stdout);
            sem_post(shmp->bufferLock);

            //the consumer leaves the critical region and increment the number of empty slots
            sem_post(shmp->empty);

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
    sem_close(shmp->bufferLock);
    sem_close(shmp->empty);
    sem_close(shmp->full);
    sem_unlink(semName1);
    sem_unlink(semName2);
    sem_unlink(semName3);
}