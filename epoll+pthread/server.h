#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/epoll.h>
#include<arpa/inet.h>

#define EPOLEN 100000
int epfd;

typedef struct task{
	void*arg;
	void*(*business)(void*);
}task_t;

typedef struct pool{
	task_t* que;
	int qmax;
	int front;
	int rear;
	int cur;

	pthread_t* ctid;
	pthread_t mtid;
	
	int pthread_max;
	int pthread_min;
	int pthread_busy;
	int pthread_alive;
	int shut_down;
	int deadcode;

	pthread_mutex_t lock;
	pthread_cond_t Not_full;
	pthread_cond_t Not_empty;
}pool_t;

pool_t* createpool(int qmax,int pmax,int pmin);
void produce_add(task_t tmp,pool_t*ptr);
void* customer(void*arg);
void* manage(void*arg);

int is_pthread_alive(pthread_t tid);

int init_Net();
int epollcreate();
void* accept_business(void*arg);
void* recv_business(void*arg);
void epoll_listen(int sockfd,pool_t*ptr);

void* test(void*arg);
