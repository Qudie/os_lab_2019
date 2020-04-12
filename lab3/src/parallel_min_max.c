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
#include <signal.h>
void al()
{
    kill(0,SIGKILL);
}
int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
int timeout =0;
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0,0},
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
            break;
          case 1:
            array_size = atoi(optarg);
            if(array_size<=0)
                exit(0);
            break;
          case 2:
            pnum = atoi(optarg);
            if(pnum<=0)
                exit(0);
            break;
          case 3:
            with_files = true;
            break;
        case 4: 
        timeout=atoi(optarg);
            if(timeout<=0)
                exit(0);
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
  printf("params: %d %d %d\n", seed, array_size, pnum);
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

    float cut = (float)array_size / pnum;
  for (i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) {
          sleep (10);
        printf("Succesfull fork: %d %d\n ", i, getpid());
        int arr_start = cut * (float)i;
        int arr_end = arr_start + cut;
        
        struct MinMax min_max1 = GetMinMax(array, arr_start, arr_end);

        if (with_files) {
             FILE *fi;
            fi=fopen("mm.txt", "w");
            char b[256];
            sprintf(b, "%d", min_max1.min);
            fprintf(fi, b);
            fprintf(fi, (const char*)"\n");
            sprintf(b, "%d", min_max1.max);
            fprintf(fi, b);
            fprintf(fi, (const char*)"\n");
            fclose(fi);
          
        } else {
            write(file_pipe[1], &min_max1.min, sizeof(int));
            write(file_pipe[1], &min_max1.max, sizeof(int));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
    if(timeout>0)
{
    signal (14,al);
    alarm(timeout);
    printf("Timeout is on %d \n", timeout);
}
  while (active_child_processes > 0) {
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
        FILE *fp;
        fp=fopen("mm.txt", "r");
        char buf[256];
        fscanf(fp,"%s",buf);
        min = atoi(buf);
        fscanf(fp,"%s",buf);
        max = atoi(buf);
    } else {
        read(file_pipe[0], &min, sizeof(int));
        read(file_pipe[0], &max, sizeof(int));
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
