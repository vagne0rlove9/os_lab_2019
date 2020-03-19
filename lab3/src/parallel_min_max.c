#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if(seed<=0)
                exit(0);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            if(array_size<=0)
                exit(0);
            // your code here
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            if(pnum<=0)
                exit(0);
            // your code here
            // error handling
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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
  int i;
  int file_pipe[2];
  if (pipe(file_pipe)<0)
  {
      exit(0);
  }
  for (i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      printf("fork1 %d %d\n", i, child_pid);
      active_child_processes += 1;
      if (child_pid == 0) {
        
        // child process
        printf("fork2 %d %d\n", i, child_pid);
        struct MinMax min_max1 = GetMinMax(array, 0, array_size);
        //printf("min: %d\n", min_max1.min);
        //printf("max: %d\n", min_max1.max);
        
        // parallel somehow
        //max
        if (with_files) {
            FILE *fp;
            fp=fopen("min_max.txt", "w");
            char buf[256];
            sprintf(buf, "%d", min_max1.min);
            fprintf(fp, buf);
            fprintf(fp, (const char*)"\n");
            sprintf(buf, "%d", min_max1.max);
            fprintf(fp, buf);
            fprintf(fp, (const char*)"\n");
            fclose(fp);
        } else {
          // use pipe here
            char buffer[256];
            sprintf(buffer,"%d",min_max1.min);
            write(file_pipe[1],buffer,strlen(buffer));
            sprintf(buffer,"%d",min_max1.max);
            write(file_pipe[1],buffer,strlen(buffer));
        }
        return 0;
      }
    } else {
      printf("fork3 %d\n", i);
      printf("Fork failed!\n");
      return 1;
    }
  }
  while (active_child_processes > 0) {
    // your code here
    wait(0);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
        FILE *fp;
        fp=fopen("min_max.txt", "r");
        char buf[256];
        fscanf(fp,"%s",buf);
        min = atoi(buf);
        fscanf(fp,"%s",buf);
        max = atoi(buf);
        printf("files\n");
    } else {    
      // read from pipes
        char buf[256];
        int num;
        read(file_pipe[0],buf,9);
        //printf("%s\n",buf);
        num=atoi(buf);
        min=num;
        read(file_pipe[0],buf,10);
        //printf("%s\n",buf);
        num=atoi(buf);
        max=num;
        printf("pipe\n");
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


//gcc parallel_min_max.c find_min_max.c utils.c && ./a.out --seed 2 --array_size 5 --pnum 1 --by_files 