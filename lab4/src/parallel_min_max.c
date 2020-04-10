#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t *child_pid;
void change()
{
    printf("killing\n");
    int i=0;
    while(child_pid[i] !=  0)
    {
    kill(child_pid[i],SIGKILL);
    i++;
    }
}
int main(int argc, char **argv) {
  int i=0;
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = 0;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 't'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
            printf("seed is a positive number\n");
            return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) {
            printf("array_size is a positive number\n");
            return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) {
            printf("pnum is a positive number\n");
            return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      case 't':
        timeout = atoi(optarg);
        if (!timeout) {
          printf("Error: bad timeout value\n");
          return -1;
        }
        break;
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }
  
  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int file_pipe[2];
  signal(SIGALRM, change);  
  alarm((unsigned)timeout);
  if (pipe(file_pipe)<0)
    {exit(0);}
  
  child_pid = malloc(sizeof(int) * pnum);
  
  for (i = 0; i < pnum; i++) {
     child_pid[i] = fork();
    if (child_pid[i] >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid[i] == 0) {
        // child process
        // parallel somehow
        struct MinMax min_max = GetMinMax(array,0,array_size);
        if (with_files) {
          FILE *f;
          f=fopen("min_max.txt","w");
          char buf[33];
          sprintf(buf,"%d",min_max.min);
          fprintf(f,buf);
          fprintf(f,"\n");
          sprintf(buf,"%d",min_max.max);
          fprintf(f,buf);
          fprintf(f,"\n");
          sleep(3);
          printf("end write\n");
  
        } else {
          // use pipe here
          
          char buf[33];
          sprintf(buf,"%d",min_max.min);
          write(file_pipe[1],buf,strlen(buf));
          sprintf(buf,"%d",min_max.max);
          write(file_pipe[1],buf,strlen(buf));
          printf("end pipe\n");
        }
        return 0;
      }
      else
      {
      
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

    int status;
  while (active_child_processes > 0) {
    // your code here
          
    wait(&status);
    active_child_processes -= 1;
  }
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      FILE *f = fopen("min_max.txt","r");
      char buf[256];
      int a;
      fscanf(f,"%s",buf);
      a=atoi(buf);
      if (min>a) min=a;
      fscanf(f,"%s",buf);
      a=atoi(buf);
      if (max<a) max=a;
    } else {
      // read from pipes
      char buf[256];
      int a;
      read(file_pipe[0],buf,9);
      a=atoi(buf);
      if (min>a) min=a;
      read(file_pipe[0],buf,10);
      a=atoi(buf);
      if (max<a) max=a;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}