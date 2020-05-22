#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)



int main(int argc, char *argv[]) {
  int fd;
  int nread;

  struct sockaddr_in servaddr;

  int BUFSIZE = -1;

  int port = -1;
  char ip[16] = {'\0'};
  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"bufsize", required_argument, 0, 0},
                                      {"ip",required_argument,0,0},
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
        BUFSIZE = atoi(optarg);
        if (!(BUFSIZE>0))
        return 0;
        // TODO: your code here
        break;
      case 2:
        strcpy(ip, optarg);
        if (!(strlen(ip)>0))
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

  if (port == -1 || BUFSIZE == -1 || strlen(ip)==0) {
    fprintf(stderr, "Using: %s --port 20001 --bufsize 4 --ip 127.0.0.1\n", argv[0]);
    return 1;
  }

  char buf[BUFSIZE];

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
    perror("bad addresss");
    exit(1);
  }

  servaddr.sin_port = htons(port);

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }
  write(fd, "hi i'll send u ", 15);
  write(fd, "10\n", 3);
  write(1, "Input message to send\n", 22);
//   while ((nread = read(0, buf, BUFSIZE)) > 0) {
//     if (write(fd, buf, nread) < 0) {
//       perror("write");
//       exit(1);
//     }
//   }
int i;
char c;
for(i = 0; i<BUFSIZE; i++){
    buf[i] = 'q';
}


//while(1){
    for(i = 0; i < 5; i++)
    {
        
        //buf1[0] = 'q';
        //nread = read(0,buf,BUFSIZE);
        nread = BUFSIZE;
        printf("%d\n", i);
        printf("%s\n", buf);
        if (write(fd, buf, nread) < 0) {
        perror("writesd");
        exit(1);
        }
    }
    write(fd, "-", 1); 
    char buf1[256];
    char buf2[256];
    // nread = read(fd, buf1, 256);
    // write(1, &buf1, nread);
    // nread = read(fd, buf1, 256);
    // write(1, &buf1, nread);
    int k = 0;
    while ((nread = read(fd, buf1, 11)) > 0) {
      
      write(1, &buf1, nread);
      printf("buf1=%s  !\n",buf1);
      
      k++;
      printf("%d\n",k);
      if(k == 2)
        break;
      for(i = 0;i<256; i++)
        buf1[i] = ' ';
    }
    printf("qqbuf1=%s  !\n",buf1);
    printf("atoibuf1=%d  !\n",atoi(buf1));
    for(i = atoi(buf1); i < 10; i++)
    {
        
        //buf1[0] = 'q';
        //nread = read(0,buf,BUFSIZE);
        nread = BUFSIZE;
        printf("i=%d\n", i);
        printf("buf1i=%s\n", buf1);
        if (write(fd, buf, nread) < 0) {
        perror("writesd");
        exit(1);
        }
    }
    //printf("%c",buf1[0]);
    printf("asd");
    
//}

  close(fd);
  exit(0);
}

//./tcpclient.o --port 10050 --bufsize 10 --ip 127.0.0.1