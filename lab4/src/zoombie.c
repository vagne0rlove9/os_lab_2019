#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>


int main(int argc, char* argv[]) {
  pid_t child = fork();

  if (child >= 0) {
    if (child > 0) {
      printf("Zombie pid=%d\n", child);
      execlp("ps","-e", NULL);
      sleep(1);
    }
    if (child == 0)
      return 0;
  }
  else {
    printf("fork() failed\n");
    return -1;
  } 
}