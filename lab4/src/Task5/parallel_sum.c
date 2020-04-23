#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include "utils.h"
#include "sum.h"

void *ThreadSum(void *args)
{
  struct SumArgs *sum_args = (struct SumArgs *)args;
  printf("Thread: [begin=%d, end=%d]\n", sum_args->begin, sum_args->end);
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
uint32_t threads_num = 0;
 uint32_t array_size = 0;
  uint32_t seed = 0;
  while (true) {
int current_optind = optind ? optind : 1;

  static struct option options[] = {{ "seed",required_argument, 0,0},
                                 {"array_size", required_argument, 0, 0},
                                 {"threads_num", required_argument, 0, 0},
                                    {0,0,0}};
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
            threads_num = atoi(optarg);
            if(!threads_num)
            {
                printf("Wrong threads_num value \n");
            return -1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
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

  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    return 1;
  }
  int a= array_size/2;
  if(threads_num >a)
  {
      printf("Too much threads, they will adopted to half array size \n");
      threads_num = a;
      
  }
  struct timeval begin_time;
  gettimeofday(&begin_time,NULL);
  float c = array_size/(float)threads_num;
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size,seed);
  pthread_t threads[threads_num];
  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = round(i*c);
    args[i].end =round((i+1)*c);

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }
  struct timeval end_time;
  gettimeofday(&end_time, NULL);
  double el_time =(end_time.tv_sec - begin_time.tv_sec)*1000;
  el_time+=(end_time.tv_usec - begin_time.tv_usec) /1000;

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}
