#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define DATA_SIZE 100

const char *IP = "192.168.110.167";
const char *PORT = "12345";

int sock;
int exitFlag;
pthread_t send_tid;
pthread_t receive_tid;

pthread_mutex_t mutx;

void interrupt(int arg)
{
	if (arg == 0)
	{
		printf("\nINFO :: Server Disconnected\n");
		printf("Bye\n");
	}
	else if (arg == 1)
	{
		printf("\nClient Close\n");
		printf("Bye\n");
	}
	else
	{
		printf("\nYou typped Ctrl + C\n");
		printf("Bye\n");
	}

	pthread_cancel(send_tid);
	pthread_cancel(receive_tid);

	pthread_join(send_tid, NULL);
	pthread_join(receive_tid, NULL);

	close(sock);
	exit(1);
}

void *sendData()
{
	char buf[DATA_SIZE];

	while (!exitFlag)
	{
		memset(buf, 0, DATA_SIZE);
		fgets(buf, DATA_SIZE - 1, stdin);
		if (!strcmp(buf, "exit\n"))
		{
			exitFlag = 1;
			write(sock, buf, strlen(buf));
			interrupt(1);
		}
		write(sock, buf, strlen(buf));
	}
}

void *receiveData()
{
	char buf[DATA_SIZE];

	while (!exitFlag)
	{
		memset(buf, 0, DATA_SIZE);
		int len = read(sock, buf, DATA_SIZE - 1);
		if (len == 0)
		{
			exitFlag = 1;
			printf("INFO :: Server Disconnected\n");
			interrupt(0);
		}
		printf("%s\n", buf);
	}
}

int main()
{
	signal(SIGINT, interrupt);

	pthread_mutex_init(&mutx, NULL);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("ERROR :: 1_Socket Create Error\n");
		exit(1);
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(atoi(PORT));

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		printf("ERROR :: 2_Connect Error\n");
		exit(1);
	}

	pthread_create(&send_tid, NULL, sendData, NULL);
	pthread_create(&receive_tid, NULL, receiveData, NULL);

	pthread_join(send_tid, 0);
	pthread_join(receive_tid, 0);

	close(sock);
	return 0;
}
