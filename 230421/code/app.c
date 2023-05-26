#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

char buf[30];

typedef struct _NODE_ {
	char n;
	char buf[100];
}Node;

int main(){
	int fd = open("/dev/nobrand", O_RDWR);
	if ( fd<0 ){
		printf("ERROR\n");
		exit(1);
	}

	// Node t = {10,"STRUCT Message!\n"};
	// ioctl(fd, _IO(0,3),&t);

	Node t;
	ioctl(fd, _IO(0,4),&t);
	printf("Copy to kernel struct!\n");
	printf("struct n : %d\n",(int)t.n);
	printf("struct buf : %s\n", t.buf);
	
	close(fd);
	return 0;
}

