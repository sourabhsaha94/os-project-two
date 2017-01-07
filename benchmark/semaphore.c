#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>


sem_t lock;

struct arg_struct{
	int count;
	int devfd;
};

void *thread_function(void *args){

	int a;
	int status =0;
    int tid;
    __u64 size;
	
    char data[4096];

	struct arg_struct *arg = args;	
	int devfd = arg->devfd;
	
	if(arg->count%3==0)	//setting
	{	
		memset(data, 0, 4096);
		a = rand();
		sprintf(data,"%d",a);
		tid = kv_set(devfd,arg->count,strlen(data),data);
		printf("arg->count is %d tid is %d\n",arg->count,tid);	
		fprintf(stderr,"S\t%d\t%d\t%d\t%s\n",tid,arg->count,strlen(data),data);
		pthread_exit(NULL);
	}
	else if(arg->count%3==1){
		tid = kv_delete(devfd,arg->count-1);
		printf("arg->count is %d tid is %d\n",arg->count,tid);	
		fprintf(stderr,"D\t%d\t%d\n",tid,arg->count);
		pthread_exit(NULL);
	}
	else{	//getting
		memset(data, 0, 4096);
		tid = kv_get(devfd,arg->count+1,&size,&data);
		printf("arg->count is %d tid is %d\n",arg->count,tid);
		fprintf(stderr,"G\t%d\t%llu\t%s\n",tid,size,data);
		pthread_exit(NULL);
	}	
}

int main(int argc, char *argv[])
{
    int i=0,number_of_threads = 10, number_of_keys=1024, number_of_transactions = 65536; 
    
	sem_init(&lock,0,1);

	struct arg_struct args[number_of_threads];	
	pthread_t t_ref[number_of_threads];
    int devfd;
	
    devfd = open("/dev/keyvalue",O_RDWR);
    if(devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }

	for(i=0;i<number_of_threads;i++)
	{
		args[i].devfd = devfd;
		args[i].count = i;
		pthread_create(&t_ref[i], NULL, &thread_function, (void *)&args[i]);
	}
	for(i=0;i<number_of_threads;i++)
	{
    	pthread_join(t_ref[i], NULL);
	}
    close(devfd);
    return 0;
}

