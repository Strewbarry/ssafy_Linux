#include <ncurses.h>
#include <unistd.h>

int main()
{
  initscr();

for (int j = 0; j < 30; j++)
{
  printw("#");
  usleep(100*1000);
}  
for (int i = 30; i >=0; i--)
{
  for (int j = 0; j <= i; j++)
  {
    printw("#");
    usleep(100*1000);
  }
  
}


  endwin();
  return 0;
}