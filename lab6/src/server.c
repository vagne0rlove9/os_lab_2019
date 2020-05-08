#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "factorial.h"


//gcc server.c -lpthread -o server.out && ./server.out --port 20001 --tnum 4

//./server.o --port 20001 --tnum 1


uint64_t Factorial(const struct FactorialArgs *args) {
    int fac = 1;
    // TODO: your code here
    int i;
    for ( i = args->begin; i <= args->end; i++)
    {
        fac*=i;
        fac%=args->mod;
        printf("%d %lu %d %lu\n", fac, args->begin, i, args->end);
    }    
    return fac;
}

void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  return (void *)(uint64_t *)Factorial(fargs);
}

int main(int argc, char **argv) {
  uint32_t i;
  int tnum = -1;
  int port = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        if (!(port>0))
        return 0;
        // TODO: your code here
        break;
      case 1:
        tnum = atoi(optarg);
        if (!(tnum>0))
        return 0;
        // TODO: your code here
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

  struct sockaddr_in server = create_sockaddr(port, htonl(INADDR_ANY));

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    fprintf(stderr, "Can not bind to socket!");
    return 1;
  }

  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];
      //pthread_t threads;

      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %llu %llu %llu\n", begin, end, mod);
      printf("tnum: %d\n", tnum);
      if((end - begin + 1) <= 3)
        tnum = 1;
      struct FactorialArgs args[tnum];
      printf("tnum: %d\n", tnum);
      //struct FactorialArgs args;
      int block = (double)(end - begin + 1)/tnum;
      args[0].end = begin + block;
      args[0].begin = begin;
      for (i = 1; i < tnum; i++) {
        // TODO: parallel somehow
        if((end - begin + 1)%2 == 0)
        {    
            block = (double)(end - begin + 1)/tnum;
            //args[i].begin = block*(i)+begin;
            args[i].begin = args[i-1].end + 1;
            args[i].end = block*(i+1)-1 + begin;
            args[i].mod = mod;
        }else 
        {
            block = (double)(end - begin + 1)/tnum;
            args[i].begin = args[i-1].end + 1;
            args[i].end = block*(i+1) + begin-1;
            args[i].mod = mod;
        }
        // args.begin = begin;
        // args.end = end;
        // args.mod = mod;

        if (pthread_create(&threads[i], NULL, ThreadFactorial,
                           (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
      for (i = 0; i < 1; i++) {
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
        total = MultModulo(total, result, mod);
      }

      printf("Total: %llu\n", total);
        
      char buffer[256];//sizeof(total)];
      sprintf(buffer, "%d", total);
      //memcpy(buffer, &total, sizeof(total));
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}