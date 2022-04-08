#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
int main() {
	int fd;
	fd=open("/dev/myPcdDev", O_RDONLY);
	if(fd==EPERM)
		printf("error opening file\n");
		
	sleep(10);
	return 0;

}
