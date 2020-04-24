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

struct f_args {
  pthread_t t;
  uint64_t begin;
  uint64_t end;
  int mod;
};
uint64_t res =1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
typedef struct f_args f_args_t;

uint64_t mul_mod(uint64_t a, uint64_t b, int mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
    }
  return result;
}

void factorial(void* arg)
{
  f_args_t* args = (f_args_t*)arg;
	uint64_t res1 = 1;
  for(uint64_t i=args->begin; i<= args->end; i++)
    res1 = mul_mod(i, res1, args->mod);
  pthread_mutex_lock(&mut);
  res = mul_mod(res1, res, args->mod);
  printf("begin: %lu end: %lu res %lu \n",args->begin,args->end,res);
  pthread_mutex_unlock(&mut);
}


int main(int argc, char **argv) {
uint64_t k = 0;
uint32_t pnum = 0;
uint32_t mod = 0;
  while (true) {
int current_optind = optind ? optind : 1;

  static struct option options[] = {{ "k",required_argument, 0,0},
                                 {"pnum", required_argument, 0, 0},
                                 {"mod", required_argument, 0, 0},
                                    {0,0,0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);
     if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if(k<=0)
                exit(0);
            break;
          case 1:
            pnum = atoi(optarg);
            if(pnum<=0)
                exit(0);
            break;
          case 2:
            mod = atoi(optarg);
            if(!mod)
            {
                printf("Wrong mod value \n");
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

  if (mod == -1 || pnum == -1 || k == -1) {
    printf("Usage: %s --mod \"num\" --pnum \"num\" --k \"num\" \n",
           argv[0]);
    return 1;
  }
  int a= k/2;
  if(pnum >a)
  {
      printf("Too much threads, they will adopted to half array size \n");
      pnum = a;
      
  }
  struct timeval begin_time;
  gettimeofday(&begin_time,NULL);
  float c = k/(float)pnum;
  f_args_t args[pnum];

  for(int i=0;i<pnum;i++)
  {
    uint64_t b =round(i*c+1);
    uint64_t e =round((i+1)*c);
    args[i].begin = b;
    args[i].end = e;
    args[i].mod = mod;

    if (pthread_create(&args[i].t, NULL,(void *)factorial, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
    
  }
  for(int i=0;i<pnum;i++)
  { 
    pthread_join(args[i].t,NULL);
  }
  
  struct timeval end_time;
  gettimeofday(&end_time, NULL);
  double el_time =(end_time.tv_sec - begin_time.tv_sec)*1000;
  el_time+=(end_time.tv_usec - begin_time.tv_usec) /1000;

 
  printf("Total: %lu\n", res);
  return 0;
}
