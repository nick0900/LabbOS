#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

struct Professor 
{
    char* name;

    int leftChopstick;
    int rightChopstick;
};

pthread_mutex_t chopSticks[5] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

int run = 1;

void LongThinking(const char* name)
{
    struct timespec req;

    float random = ((float) rand()) / (float) RAND_MAX;

    float randTime = 2 + 8 * random;
    req.tv_sec = (float)floor(randTime);

    randTime -= (float)floor(randTime);;

    req.tv_nsec = 999999999 * randTime;

    struct timespec rem;

    printf("%s is Thinking\n",name);

    nanosleep(&req, &rem);
}

void ShortThinking(const char* name)
{
    struct timespec req;

    float random = ((float) rand()) / (float) RAND_MAX;

    float randTime = 1 + 3 * random;
    req.tv_sec = (float)floor(randTime);;

    randTime -= (float)floor(randTime);;

    req.tv_nsec = 999999999 * randTime;

    struct timespec rem;

    printf("%s is Thinking\n",name);

    nanosleep(&req, &rem);
}

void Eating(struct Professor* prof)
{
    struct timespec req;

    float random = ((float) rand()) / (float) RAND_MAX;

    float randTime = 10 + 10 * random;
    req.tv_sec = (float)floor(randTime);;

    randTime -= (float)floor(randTime);;

    req.tv_nsec = 999999999 * randTime;

    struct timespec rem;

    printf("%s is eating\n",prof->name);

    nanosleep(&req, &rem);

    pthread_mutex_unlock(&(chopSticks[prof->leftChopstick]));
    pthread_mutex_unlock(&(chopSticks[prof->rightChopstick]));

    printf("%s put down chopsticks\n",prof->name);
}

void TakeLeft(struct Professor* prof)
{
    printf("%s is waiting to take left\n",prof->name);

    pthread_mutex_lock(&(chopSticks[prof->leftChopstick]));

    printf("%s has aquired the left chopstick\n",prof->name);

    ShortThinking(prof->name);
}

void TakeRight(struct Professor* prof)
{
    printf("%s is waiting to take right\n",prof->name);

    pthread_mutex_lock(&(chopSticks[prof->rightChopstick]));

    printf("%s has aquired the right chopstick\n",prof->name);
}

void* ThreadWork(void* buf) {
    struct Professor* prof = (struct Professor*) buf;

    while (run == 1)
    {
        LongThinking(prof->name);
        TakeLeft(prof);
        TakeRight(prof);
        Eating(prof);
    }
    printf("%s has left\n",prof->name);

    return NULL;
}

int main(void)
{
    pthread_t profThreads[5];

    struct Professor professors[5];

    professors[0].name = "Albert";
    professors[1].name = "Bertil";
    professors[2].name = "Carl";
    professors[3].name = "Dennis";
    professors[4].name = "Erik";

    for (int i = 0; i < 5; i++)
    {
        professors[i].leftChopstick = i;
        if (i < 5 - 1)
        {
            professors[i].rightChopstick = i + 1;
        }
        else
        {
            professors[i].rightChopstick = 0;
        }
        pthread_create(&profThreads[i], NULL, ThreadWork, &professors[i]);
    }
    
    struct timespec req;
    req.tv_sec = 60;
    req.tv_nsec = 0;

    struct timespec rem;

    nanosleep(&req, &rem);
    run = 0;
    printf("Lunch over\n");
    
    for (int i = 0; i < 5; i++)
    {
        pthread_join(profThreads[i], NULL);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_mutex_destroy(&(chopSticks[i]));
    }

    return 0;
}