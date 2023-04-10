#include <ncurses.h>
#include <unistd.h>
int main()
{
  initscr();
  
  for (int i = 0; i < 27; i++)
  {
    for (int j = 0; j <= i; j++)
    {
      printw("%c",'A'+j);
      refresh();
    }
    usleep(100*1000);
    printw("\n");   
  }
  

  endwin();

  return 0;
}
