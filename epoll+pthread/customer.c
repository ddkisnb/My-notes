#include<server.h>

void* customer(void*arg){
	pthread_detach(pthread_self());//使线程结束时释放资源
	
	pool_t* ptr = (pool_t*)arg;
	task_t tmp;

	while(!ptr->shut_down){
		pthread_mutex_lock(&ptr->lock);
		while(ptr->cur==0){
			pthread_cond_wait(&ptr->Not_empty,&ptr->lock);
			if(ptr->shut_down){
				ptr->pthread_alive--;
				printf("thread exit\n");
				pthread_mutex_unlock(&ptr->lock);
				pthread_exit(NULL);
			}

//检查是否需要销毁线程
			if(ptr->deadcode>0){
				ptr->deadcode--;
				ptr->pthread_alive--;
				pthread_mutex_unlock(&ptr->lock);
				pthread_exit(NULL);
			}
		}
//从队列尾部(rear)取出任务：
		tmp.business = ptr->que[ptr->rear].business;
		tmp.arg = ptr->que[ptr->rear].arg;
		ptr->pthread_busy++;
		printf("customer busy:%d\n",ptr->pthread_busy);
		ptr->cur--;
		ptr->rear = (ptr->rear+1)%ptr->qmax;
		pthread_mutex_unlock(&ptr->lock);
//消费完，设置一个不空的信号
		pthread_cond_signal(&ptr->Not_full);

//打印当前线程ID和执行任务信息
		printf("thread[0x%x] do tmp\n",(unsigned int)pthread_self());
		tmp.business(tmp.arg);
		
		
		pthread_mutex_lock(&ptr->lock);
		ptr->pthread_busy--;
		pthread_mutex_unlock(&ptr->lock);
	}
	return NULL;
}
