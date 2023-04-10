#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define N 20

pthread_mutex_t mlock;

char map[N][N + 1] = {
    "##########",
    "#        #",
    "#        #",
    "#      a #",
    "#    ^^^^#",
    "#        #",
    "#     Y  #",
    "# #      #",
    "# #      #",
    "##########"};

int ny = 1;
int nx = 1;
int my = 1;
int mx = 5;
int yy, yx;
int hp = 100;

void print()
{
  clear();
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (i == ny && j == nx)
        printw("さ");
      else if (i == my && j == mx)
        printw("M ");
      else if (map[i][j] == ' ')
        printw("  ");
      else if (map[i][j] == '#')
        printw("▩ ");
      else if (map[i][j] == 'Y')
        printw("YY");
      else if (map[i][j] == '^')
        printw(" Δ");
      else if (map[i][j] == 'a')
        printw(" α");
    }
    printw("\n");
  }
  printw(" HP : %d\n", hp);
  refresh();
}

void *monstormove()
{
  srand(time(NULL));
  int dy[4] = {-1, 1, 0, 0};
  int dx[4] = {0, 0, -1, 1};

  while (1)
  {
    usleep(200 * 1000);
    int dir = rand() % 4;
    int nny = my + dy[dir];
    int nnx = mx + dx[dir];
    if (map[nny][nnx] == '#')
      continue;
    my = nny;
    mx = nnx;
  }
  // print();
  // mvprintw(6,8,"my : %d mx : %d",my,mx);
}

int main()
{
  setlocale(LC_CTYPE, "ko_KR.utf8");
  initscr();

  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  pthread_t tid;
  pthread_mutex_init(&mlock, NULL);
  pthread_create(&tid, NULL, monstormove, NULL);

  while (1)
  {
    print();

    int ch = getch();
    if (ch == ERR)
      ch = 0;

    if (ch == KEY_LEFT)
    {
      if (map[ny][nx - 1] != '#')
        nx--;
      if (map[ny][nx] == '^')
        hp -= 10;
    }
    if (ch == KEY_RIGHT)
    {
      if (map[ny][nx + 1] != '#')
        nx++;
      if (map[ny][nx] == '^')
        hp -= 10;
    }
    if (ch == KEY_UP)
    {
      if (map[ny - 1][nx] != '#')
        ny--;
      if (map[ny][nx] == '^')
        hp -= 10;
    }
    if (ch == KEY_DOWN)
    {
      if (map[ny + 1][nx] != '#')
        ny++;
      if (map[ny][nx] == '^')
        hp -= 10;
    }

    if (map[ny][nx] == 'a')
    {
      hp += 10;
      map[ny][nx] = ' ';
    }
    if ((ny == my && nx == mx) || map[ny][nx] == 'Y' || hp == 0)
    {
      print();
      break;
    }
  }
  usleep(300 * 1000);
  clear();
  if ((ny == my && nx == mx) || hp == 0)
    mvprintw(5, 10, "GAME OVER");
  else if (map[ny][nx] == 'Y')
  {
    mvprintw(5, 10, "YOU WIN\n");
    mvprintw(6, 8, " HP : %d", hp);
  }
  refresh();
  usleep(1000 * 1000);
  clear();
  getch();  pthread_join(tid, NULL);
  endwin();

  return 0;
}