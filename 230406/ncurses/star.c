#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  initscr();

  srand(time(NULL));

  while(1)
  {
    int y = rand() % 45;
    int x = rand() % 120;
    move(y,x);
    printw("*");
    refresh();
    usleep(10*1000);
  }
  


  getch();
  endwin();

  return 0;
  
}