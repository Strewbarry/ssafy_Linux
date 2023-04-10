#include <stdio.h>
#include <time.h>


int main(){

  time_t current_time = time(NULL);

  struct tm *ttm = localtime(&current_time);

  printf("%d/%d/%d %d\n",ttm->tm_year+1900,ttm->tm_mon,ttm->tm_mday,ttm->tm_wday);
  printf("%d : %d : %d\n",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);

  return 0;  
}

