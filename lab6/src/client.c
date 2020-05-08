#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <math.h>
#include "MultModulo.h"

struct Server {
  char ip[255];
  int port;
  struct Server* next;
  pthread_t id;
  int socket;
  uint64_t answer;
};
int count =0;


struct Server* conv_ip(const char* server )
{
  printf("Reading from %s\n", server);
  FILE* servers = fopen(server,"r");
  if(servers == NULL) {
    printf("Error opening %s\n", server);
    return 0;
  }
  struct Server* prev = NULL;
  struct Server* first = NULL;
  while(1)
  {   
    struct Server* Friday = malloc(sizeof(struct Server));
    int res = fscanf(servers,"%s : %d", Friday->ip, &Friday->port);
    if(res == 0 || res == EOF)
    {
      prev->next=NULL;
      free(Friday);
      break;
    }
    if(prev==NULL)
    {
      prev = Friday;
      first = Friday;
    }
    else
    {
      prev->next = Friday;
      prev = Friday;
    }
    printf("Read: %s:%d to %p\n", Friday->ip, Friday->port, Friday);
    count ++;
  }
  fclose(servers);
  return first;
}

void recieve(void* arg)
{
  struct Server* a = (struct Server*)arg;
  printf("Waiting: [%s:%d sck:%d, t_id:%lu]\n", a->ip, a->port, a->socket, a->id);
  char response[sizeof(uint64_t)];
  if (recv(a->socket, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }
  else {
    memcpy(&a->answer, response, sizeof(uint64_t));
    printf("Done: [%s:%d sck:%d, t_id:%lu, res=%lu]\n", a->ip, a->port, a->socket, a->id, a->answer);
  }
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        if(ConvertStringToUI64(optarg, &k) == false)
        {printf("Error in k \n");
        exit(1);}
        break;
      case 1:
       if( ConvertStringToUI64(optarg, &mod)== false)
        {printf("Error in mod \n");
        exit(1);}
        break;
      case 2:
        if(memcpy(servers, optarg, strlen(optarg))==false)
        {printf("Error \n");
        exit(1);}
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  
  struct Server* m = conv_ip(servers);
  struct Server* m_1 = m;
  int i=0;
  float c = k/(float)count;
  while(m)
  {
  printf("Connintion: [ip: %s:%d] from %p\n", m->ip, m->port, m);

    struct hostent *hostname = gethostbyname(m->ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", m->ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(m->port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    m->socket = sck;
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    uint64_t b =round(i*c+1);
    uint64_t e =round((i+1)*c);
    char task[sizeof(uint64_t) * 3];
    memcpy(task, &b, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &e, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    if (pthread_create(&m->id, NULL, (void *)recieve, (void *)m)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }

    m = m->next;
    i++;
  }

  uint64_t answer = 1;
  while(m_1)
  {
    pthread_join(m_1->id,NULL);
    answer *= m_1->answer;
    close(m_1->socket);
    m_1 = m_1->next;
  }

  printf("Answer: %lu\n", answer);
  return 0;
}
