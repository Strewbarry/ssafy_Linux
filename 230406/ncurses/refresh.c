#include <ncurses.h>
#include <unistd.h>

int main()
{
  initscr();

  for (int i = 3; i >= 0; i--)
  {
    printw("[%d]",i);
    refresh();
    sleep(1);
    clear();
  }

  for (int i = 0; i < 15; i++)
  {
    printw("=");
    refresh();
  }
  printw("\nGo GO GO GO GO GO\n");
  
  for(int i=0;i<15;i++){
   printw("=");
   refresh();
  }
    

  sleep(2);

  endwin();
  return 0;  
  
}