#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void *abc()
{
  int num = 0;
  while (1)
  {
    mvprintw(10, 10, "            ");
    mvprintw(10, 10, "%d", num);
    refresh();
    num--;
  }
}

int main()
{
  initscr();

  clear();

  pthread_t tid;
  pthread_create(&tid, NULL, abc, NULL);

  int num = 0;
  while (1)
  {
    mvprintw(10, 10, "            ");
    mvprintw(10, 10, "%d", num);
    refresh();
    num++;
  }
  pthread_join(tid,NULL);

  getch();
  endwin();

  return 0;
}
