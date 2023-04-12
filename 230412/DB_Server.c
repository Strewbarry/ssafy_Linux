#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENT_CNT 500

// 단일 주소 서버임을 가정하여 포트 번호만 표기
const char *PORT = "12345";

// File Descriptor
// 서버와 클라이언트 양쪽 소켓에 대한 파일
int server_sock;
int client_sock[MAX_CLIENT_CNT];
struct sockaddr_in client_addr[MAX_CLIENT_CNT];
int exitFlag[MAX_CLIENT_CNT];

pthread_t tid[MAX_CLIENT_CNT];
pthread_mutex_t mutx;

void postToDB(char *key, char *value, int sockId);
char *getFromDB(char *key, int sockId);

// 인터럽트로 인한 종료 시나리오
void interrupt(int arg)
{
    printf("\nYou typed Ctrl + C\n");
    printf("Bye\n");

    for (int i = 0; i < MAX_CLIENT_CNT; i++)
    {
        if (client_sock[i] != 0)
        {
            pthread_cancel(tid[i]);
            pthread_join(tid[i], 0);
            close(client_sock[i]);
        }
    }
    close(server_sock);
    exit(1);
}

void removeEnterChar(char *buf)
{
    int len = strlen(buf);
    for (int i = len - 1; i >= 0; i--)
    {
        if (buf[i] == '\n')
            buf[i] = '\0';
        break;
    }
}

int getClientID()
{
    for (int i = 0; i < MAX_CLIENT_CNT; i++)
    {
        if (client_sock[i] == 0)
            return i;
    }
    return -1;
}

void postToDB(char *key, char *value, int sockId)
{
    // 이미 변수가 존재한다면
    FILE *fd;
    fd = fopen("database.txt", "r");
    char file_buff[100];
    if (fd != NULL)
    {
        while (fgets(file_buff, sizeof(file_buff), fd) != NULL)
        {
            char *p = strtok(file_buff, " ");
            if (strcmp(p, key) == 0)
            {
                const char *msg = "ERROR :: ALREADY EXIST KEY\nINPUT ANOTHER KEY\n";
                write(client_sock[sockId], msg, strlen(msg));

                fclose(fd);
                return;
            }
        }
    }
        
    // 없는 변수라면
    int fp = open("./database.txt", O_WRONLY, 0666);
    if (fp < 0)
    {
        fp = open("./database.txt", O_CREAT, 0666);
        close(fp);
        fp = open("./database.txt", O_WRONLY, 0666);
    }

    lseek(fp, 0, SEEK_END);
    lseek(fp, 0, SEEK_CUR);

    char line[100];
    sprintf(line, "%s %s\n", key, value);
    write(fp, line, strlen(line));
    close(fp);
}

char *getFromDB(char *key, int sockId)
{
    FILE *fp;
    fp = fopen("database.txt", "r");
    if (fp == NULL)
    {
        char *msg = "ERROR :: NO DB EXISTS\n";

        return msg;
    }
    char file_buff[100];

    int n = 0;
    char *rtn;
    while (fgets(file_buff, sizeof(file_buff), fp) != NULL)
    {
        n++;
        char *p = strtok(file_buff, " ");
        if (strcmp(p, key) == 0)
        {
            p = strtok(NULL, " ");
            sprintf(rtn, "[%d] %s :: %s", n, key, p);
            return rtn;
        }
    }
    sprintf(rtn, "KEY NOT FOUND");
    fclose(fp);
    return rtn;
}

void *client_handler(void *arg)
{
    int id = *(int *)arg;
    char IPaddr[100];

    strcpy(IPaddr, inet_ntoa(client_addr[id].sin_addr));
    printf("INFO :: Connect new Client (ID : %d, IP : %s)\n", id, IPaddr);

    char buf[100];
    while (1)
    {
        memset(buf, 0, 100);
        int len = read(client_sock[id], buf, 99);
        if (len == 0)
        {
            printf("INFO :: Disconnect with client.. BYE\n");
            exitFlag[id] = 1;
            break;
        }

        if (!strcmp("exit", buf))
        {
            printf("INFO :: Client want close.. BYE\n");
            exitFlag[id] = 1;
            break;
        }

        removeEnterChar(buf);

        // Database 처리
        // 명령어 형식은 "GET/POST [변수명] [값]"
        pthread_mutex_lock(&mutx);
        char *cmd = strtok(buf, " ");
        if (strcmp(cmd, "POST") == 0)
        {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, " ");

            postToDB(key, value, id);
        }
        else if (strcmp(cmd, "GET") == 0)
        {
            char *key = strtok(NULL, " ");
            char *result = getFromDB(key, id);
            write(client_sock[id], result, strlen(result));
        }
        else
        {
            char *msg = "ERROR :: INVALID COMMAD\n";
            write(client_sock[id], msg, strlen(msg));
        }
        pthread_mutex_unlock(&mutx);
    }
    close(client_sock[id]);
}

int main()
{
    // SIGINT, 통상적으로 Ctrl + C => 종료 인터럽트
    signal(SIGINT, interrupt);

    // 서버 소켓 생성
    server_sock = socket(PF_INET, SOCK_STREAM, 0);

    pthread_mutex_init(&mutx, NULL);

    if (server_sock == -1)
    {
        printf("ERROR :: 1_Socket Create Error\n");
        exit(1);
    }

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(optval));

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(PORT));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("ERROR :: 2_bind Error\n");
        exit(1);
    }

    if (listen(server_sock, 5) == -1)
    {
        printf("ERROR :: 3_listen Error");
        exit(1);
    }

    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    int id_table[MAX_CLIENT_CNT];

    while (1)
    {
        int id = getClientID();
        id_table[id] = id;

        if (id == -1)
        {
            printf("WARNING :: CLIENT FULL\n");
            sleep(1);
        }

        memset(&client_addr[id], 0, sizeof(struct sockaddr_in));

        client_sock[id] = accept(server_sock, (struct sockaddr *)&client_addr[id], &client_addr_len);
        if (client_sock[id] == -1)
        {
            printf("ERROR :: 4_accept Error\n");
            break;
        }

        pthread_create(&tid[id], NULL, client_handler, (void *)&id_table[id]);

        for (int i = 0; i < MAX_CLIENT_CNT; i++)
        {
            if (exitFlag[i] == 1)
            {
                exitFlag[i] = 0;
                pthread_join(tid[i], 0);
                client_sock[i] = 0;
            }
        }
    }

    close(server_sock);
    return 0;
}