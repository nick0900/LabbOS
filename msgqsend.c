#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h> 

#define PERMS 0644
#define INTS 50
#define INT_MIN -10
#define INT_MAX 10

struct my_msgbuf 
{
    long mtype;
    int mtext;
};

int main(void) 
{
    struct my_msgbuf buf;
    int msqid;
    int len = sizeof(int);
    key_t key;
    system("touch msgq.txt");
    
    if ((key = ftok("msgq.txt", 'B')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    
    if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }
    
    printf("message queue: ready to send messages.\n");
    printf("Send ints? enter any character. exit with q:\n");
    buf.mtype = 1;
    
    /*we don’t really care in this case*/
    
    while (getchar() != 'q') 
    {
        for (int i = 0; i < INTS; i++)
        {
            float randfrac = (float)rand() / (float)RAND_MAX;
            buf.mtext = (int)(INT_MIN + (INT_MAX - INT_MIN) * randfrac);
            printf("Sent: %i ", buf.mtext);
            
            if(msgsnd(msqid, &buf, len, 0) == -1)
            perror("msgsnd");
        }
    }
    
    buf.mtype = 2;
    
    if(msgsnd(msqid, &buf, len, 0) == -1)
    perror("msgsnd");

    if(msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        exit(1);
    }
    printf("message queue: done sending messages.\n");

    return 0;
}