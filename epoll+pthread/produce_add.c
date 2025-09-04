#include<server.h>

void produce_add(task_t tmp,pool_t*ptr){
	if(!ptr->shut_down){
		pthread_mutex_lock(&ptr->lock);
		while(ptr->cur==ptr->qmax){
			//等待不满了，开始生产
			pthread_cond_wait(&ptr->Not_full,&ptr->lock);
			if(ptr->shut_down){
				pthread_mutex_unlock(&ptr->lock);
				pthread_exit(NULL);
			}
		}

		ptr->que[ptr->front].business = tmp.business;
		ptr->que[ptr->front].arg = tmp.arg;
		ptr->cur++;
		ptr->front = (ptr->front+1)%ptr->qmax;
		pthread_mutex_unlock(&ptr->lock);
		//信号
		pthread_cond_signal(&ptr->Not_empty);
		
	}else{
		printf("shut_down:%d\n",ptr->shut_down);
		pthread_exit(NULL);
	}
	printf("produce_add success\n");
}
