#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "factorial.h"


//gcc client.c -o client.out && ./client.out --k 1000 --mod 5 --servers servers.txt

//./client.out --k 5 --mod 19 --servers servers.txt


struct Server {
  char ip[255];
  int port;
  struct FactorialArgs args; 
};

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
  int i;
  int j;
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
        ConvertStringToUI64(optarg, &k);
        if (!(k>0))
        return 0;
        // TODO: your code here
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        if (!(mod>1))
        return 0;
        // TODO: your code here
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
        // ???
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

  // TODO: for one server here, rewrite with servers from file
  // read from file
  
  unsigned int servers_num = 0;
  struct Server *to;// = malloc(sizeof(struct Server) * servers_num);
    FILE *f = fopen("servers.txt","r");
    char buf[255];
    while(fgets(buf,256,f) != NULL)
    {servers_num++;}
    fclose(f);
  //
  
  to = malloc(sizeof(struct Server)*servers_num);
    f = fopen("servers.txt","r");
    int block = (double)k/servers_num;
    for (i=0;i<servers_num;i++)
    {
        fscanf(f,"%s",buf);
        //printf("%s\n",buf);
        memcpy(to[i].ip,buf,sizeof(buf));
        fscanf(f,"%s",buf);
        //printf("%s\n",buf);
        fscanf(f,"%s",buf);
        //printf("%s\n",buf);
        to[i].port=atoi(buf);   
        to[i].args.begin = ((double)k/servers_num)*(i)+1;
        to[i].args.end = ((double)k/servers_num)*(i+1);
        to[i].args.mod = mod;
    }
  // TODO: delete this and parallel work between servers

  //to[0].port = 20001;
  //memcpy(to[0].ip, "127.0.0.1", sizeof("127.0.0.1"));

  // TODO: work continiously, rewrite to make parallel
  // ???
    

    int active_child_processes = 0;
    uint64_t answer = 1;
    uint64_t temp=0;
    int file_pipe[2];
    if (pipe(file_pipe)<0)
        {exit(0);}

    for (i=0;i<servers_num;i++)
    {
    
    //printf("%d\n",i);
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
        if (child_pid == 0) {
            struct hostent *hostname = gethostbyname(to[i].ip);
            if (hostname == NULL) {
                fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
                exit(1);
            }

            struct sockaddr_in server = create_sockaddr(to[i].port, *((unsigned long *)hostname->h_addr ));
            
            int sck = socket(AF_INET, SOCK_STREAM, 0);
            if (sck < 0) {
                fprintf(stderr, "Socket creation failed!\n");
                exit(1);
            }

            if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
                fprintf(stderr, "Connection failed\n");
                exit(1);
            }

            // TODO: for one server
            // parallel between servers
            char task[sizeof(uint64_t) * 3];
            memcpy(task, &to[i].args.begin, sizeof(uint64_t));
            memcpy(task + sizeof(uint64_t), &to[i].args.end, sizeof(uint64_t));
            memcpy(task + 2 * sizeof(uint64_t), &to[i].args.mod, sizeof(uint64_t));

            if (send(sck, task, sizeof(task), 0) < 0) {
                fprintf(stderr, "Send failed\n");
                exit(1);
            }
            
            char response[sizeof(uint64_t)];
            if (recv(sck, response, sizeof(response), 0) < 0) {
                fprintf(stderr, "Recieve failed\n");
                exit(1);
            }
            
            // TODO: from one server
            // unite results
            memcpy(&buf, response, sizeof(uint64_t));
            
            //printf("%s\n",buf);
            flock(file_pipe,"LOCK_EX");
            //printf("%s\n",buf);
            write(file_pipe[1],buf,strlen(buf));
            write(file_pipe[1],"\n",1);
            flock(file_pipe,"LOCK_UN");
            close(sck);
            //printf("%s\n",buf);
            return 0;
            }
        }
        else {
        printf("Fork failed!\n");
        return 1;
        }
    }
    int status;
    while (active_child_processes > 0) {
    // your code here
        wait(&status);
        //printf("%d\n", active_child_processes);
        active_child_processes -= 1;
    }
    //printf("%d\n", active_child_processes);
    //printf("----------\n");
    //printf("%d\n", servers_num);
    //for (i=0;i<servers_num;i++)
    //{

        //printf("%d\n",servers_num);
        //printf("%d\n", i);
        char buf1[255];
        read(file_pipe[0],buf1,10);
        //printf("%s\n", buf1);
       
        int numBegin = 0;
        int numEnd = 0;
        for (i=0;i<servers_num;i++)
        {
            char f = buf1[numBegin];
            char number[255];
            while(f!='\n')
            {
                number[numEnd]=buf1[numEnd];
                numEnd++;
                f = buf1[numEnd];               
            }
            numEnd+=1;
            numBegin = numEnd;

            temp=atoi(number);
            
            for(j=0;j<255;j++){
                number[j]=' ';            
            }
            printf("%d\n", temp);
            answer = MultModulo(temp, answer, mod);
        }
        //printf("%lu\n", temp);
        //printf("%d\n",temp);        
        //printf("%d\n", i);
    //}
    printf("answer: %llu\n", answer);
  free(to);
  return 0;
}