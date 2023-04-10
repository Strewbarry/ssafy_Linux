#include <stdio.h>
#include <sys/time.h>

int main()
{
  struct timeval time;

  gettimeofday(&time,NULL);

  printf("%ld\n",time.tv_sec);
  printf("%ld\n",time.tv_usec);

  return 0;
  
}