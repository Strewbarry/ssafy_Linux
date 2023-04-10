#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  initscr();

  srand(time(NULL));

  for (int i = 0; i < 10; i++)
  {
    printw("%d\n",rand() % 10);
  }
  refresh();

  getch();
  endwin();

  return 0;
  
}