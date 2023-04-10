#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

char vect[4][21] = {
  "ABCDFGHIJKLMNOPQRST",
  "HIFAKERHIFAKERHIFAK",
  "BBQBBQBBQBBQBBQBBQB",
  "MACMACMACMACMACMACM"
};

void *abd(void *a){
  int aa = *(int *)a;
  for (int i = 0; i < 20; i++)
  {
    printf("%c",vect[aa][i]+3);
  }
  printf("\n");  
}

int main(){
  pthread_t t[4];
  int arr[4]={0,};

  for (int i = 0; i < 4; i++)
  {
    arr[i] = i;
    pthread_create(&t[i],NULL,abd,&arr[i]);
  }
  for (int i = 0; i < 4; i++)
  {
    pthread_join(t[i],NULL);
  }
  return 0;
  
}
