#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void gogo(){
  printf("WAKE UP!\n");
  exit(1);
}

int main(){
  signal(SIGALRM,gogo);
  alarm(3);

  printf("3 seconds\n");
  printf("wait...\n");

  while(1) sleep(1);

  return 0;
}