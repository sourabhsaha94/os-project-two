#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[])
{
    int i=0,number_of_threads = 1, number_of_keys=3, number_of_transactions = 65536; 
    int a;
    int tid;
    __u64 size;
    char data[4096];
    int devfd;
    /*if(argc < 3)
    {
        fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n",argv[0]);
        exit(1);
    }
    number_of_keys = atoi(argv[1]);
    number_of_transactions = atoi(argv[2]);*/
    devfd = open("/dev/keyvalue",O_RDWR);
    if(devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }

    	memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,565,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,565,strlen(data),data);

        memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,22,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,22,strlen(data),data);

        memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,90,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,90,strlen(data),data);
        
		memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,90,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,90,strlen(data),data);
        
		memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,91,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,91,strlen(data),data);

        memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,0,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,0,strlen(data),data);


        tid = kv_delete(devfd,90);

		 memset(data,0,4096);
        tid = kv_get(devfd,91,&size,&data);
        fprintf(stderr,"G\t%d\t%llu\t%s\n",tid,size,data);

    /*for(i = 0; i < number_of_keys; i++)
    {
        memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,i,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,i,strlen(data),data);
    }*/
    /*for(i = 0; i < number_of_keys; i++)
    {
        memset(data, 0, 4096);
        a = rand();
        sprintf(data,"%d",a);
        tid = kv_set(devfd,i,strlen(data),data);
        fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,i,strlen(data),data);
    }*/
    /*for(i = 0; i < number_of_transactions; i++)
    {
        tid = kv_get(devfd,i,size,&a);
        fprintf(stderr,"G\t%d\t%d\t%s\n",tid,sizeof(int),a);
        
    }*/
    close(devfd);
    return 0;
}

