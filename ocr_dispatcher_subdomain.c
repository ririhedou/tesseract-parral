//author ketian
//ririhedou@gmail.com

#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define WATHEVER_CMD_PREFIX "timeout 10s tesseract  %s  %s"

#define PROC_MAX 20
#define DATA_SIZE 128
//The maximum processes the machine can tolerate
//10X Fast


void work_tesseract(char* img)
{
      int len = strlen(img);
      char output[1024];
      if(len == 0) return;
      strcpy(output, img);
      output[len-4] = '\0';
      char buf[2056];
      snprintf(buf, 2056, WATHEVER_CMD_PREFIX, img, output);
      //use snprintf to avoid buffer overflow
      //printf("%s\n", buf);
      system(buf);

}

// /home/ketian/tmp/  idx=118
void work_on_idx_web(int idx, char* dir)
{
      char cmd[2056];
      char data[DATA_SIZE];
      FILE *pf;
      snprintf(cmd, 2056, "find %s%d -type f -name \"*.png\"", dir, idx);

      //printf("cmd is %s\n",cmd);
      // Setup our pipe for reading and execute our command.
      pf = popen(cmd,"r");

      if(!pf){
         fprintf(stderr, "Could not open pipe for output.\n");
         return;
      }

      // Grab data from process execution
      while (fgets(data, sizeof data, pf) != NULL)
      {
            size_t len = strlen(data);
            if (len > 0 && data[len-1] == '\n') {
                  data[--len] = '\0';
            }
            //printf("fuck %s",data);
            work_tesseract(data);
      }

      return ;
}


//sample command as:
//./a.out 1 4
int main(int argc, char* argv[])
{

    //char img[] = "/home/ketian/tmp/40_mobile/your-answers-here.net..screen.44.png";
    if (argc < 2) {
        /* no argument */
        /* give error message and exit */
        fprintf(stderr, "Must pass an argument!\n");
        exit(1);
    }

    //work_tesseract(argv[1]);
    int ppid = getpid();

    if (argc<3)
    {
        //usage();
        exit(-1);
    }
    int start_val = atoi(argv[1]);
    int stop_val = atoi(argv[2]);
    char* dir = argv[3];

    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid<0)
    {
        exit(-1);
    }

    signal(SIGCHLD, SIG_IGN);
    //to prevent zombie processes

    int* counter;
    counter = shmat(shmid, NULL, 0);
    *counter = 0;

    for (int i=start_val; i<=stop_val; i++)
    {
        int cpid;
        //retry counter
        int rcnt = 0;
        //respwaning new task, increase counter by one
        while(!__sync_bool_compare_and_swap(counter,*counter, (*counter) +1));
retry:
        cpid = fork();
        rcnt++;
        if (cpid==0)
        {
            work_on_idx_web(i, dir);
            while(!__sync_bool_compare_and_swap(counter,*counter, (*counter)-1));
            printf("[SPONGEBOB]Done %d counter=%d\n", i, *counter);
            shmdt(counter);
            exit(-1);
        }
        else if (cpid==-1)
        {
            //failed to fork...
            printf("[SPONGEBOB]Fail to fork on id=%d...\n", i);
            if (rcnt>100)
            {
                goto out; //we re-try 100 times
            }
            goto retry;
        }
out:
        while (*counter>=PROC_MAX)
        {
            sleep(10);
        }
    }

    printf("[SPONGEBOB] Done and exit\n");
    exit(0);

}
