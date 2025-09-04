#include<server.h>

pool_t* createpool(int qmax,int pmax,int pmin){
	pool_t* ptr = (pool_t*)malloc(sizeof(pool_t));
	ptr->que = (task_t*)malloc(sizeof(task_t));
	ptr->qmax = qmax;
	ptr->front = 0;
	ptr->rear = 0;
	ptr->cur =0;

	ptr->ctid = (pthread_t*)malloc(sizeof(pthread_t)*pmax);
	bzero(ptr->ctid,sizeof(pthread_t)*pmax);
	ptr->pthread_max = pmax;
	ptr->pthread_min = pmin;
	ptr->pthread_busy =0;
	ptr->pthread_alive = 0;
	ptr->deadcode =0;
	ptr->shut_down =0;

	if(pthread_mutex_init(&ptr->lock,NULL)!=0||pthread_cond_init(&ptr->Not_empty,NULL)!=0||pthread_cond_init(&ptr->Not_full,NULL)!=0){
		perror("init lock failed\n");
		return NULL;
	}

	for(int i=0;i<pmin;i++){
		if(pthread_create(&ptr->ctid[i],NULL,customer,(void*)ptr)!=0){
			perror("pthread_create failed\n");
			return NULL;
		}	
		ptr->pthread_alive++;
	}

	if(pthread_create(&ptr->mtid,NULL,manage,(void*)ptr)!=0){
		perror("pthread_create failed\n");
		return NULL;
	}
	return ptr;
}
