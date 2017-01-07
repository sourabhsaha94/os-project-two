#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int i=0,number_of_threads = 1, number_of_keys=1024; 
    int tid;
    __u64 size;
    __u64 key;
    char data[4096],test[4096],op;
    char **kv;
    int devfd;
    int error = 0;
    
    devfd = open("/dev/keyvalue",O_RDWR);
    if(devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }
    
	printf("Key: \n");
	scanf("%llu",&key);
	printf("Value: \n");
	scanf("%s",&test);
	memset(data,0,4096);
	tid = kv_get(devfd,i,&size,&data);
	if(strcmp(data,test)==0){
		printf("You passed\n");
	}else{
		printf("You failed\n");
	}
    close(devfd);
    return 0;
}


