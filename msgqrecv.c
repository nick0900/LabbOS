#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
#define INTS 50

struct my_msgbuf {
   long mtype;
   int mtext;
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to receive messages.\n");

   for(;;) { /* normally receiving never ends but just to make conclusion */
             /* this program ends with mtype == 0 */
      if (msgrcv(msqid, &buf, sizeof(int), 0, 0) == -1) {
         perror("msgrcv");
         exit(1);
      }

      if (buf.mtype == 2)
      break;

      printf("recvd: ");
      printf("%i ", buf.mtext);
      printf("\n");
   }
   printf("message queue: done receiving messages.\n");
   system("rm msgq.txt");
   return 0;
}
