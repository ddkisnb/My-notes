#include<server.h>
void*manage(void*arg){
	pthread_detach(pthread_self());
	pool_t* ptr = (pool_t*)arg;
	int add,i;

	while(!ptr->shut_down){
		
		pthread_mutex_lock(&ptr->lock);
		
		int cur = ptr->cur;
		int pmax = ptr->pthread_max;
		int pmin = ptr->pthread_min;
		int busy = ptr->pthread_busy;
		int alive = ptr->pthread_alive;
		
		pthread_mutex_unlock(&ptr->lock);


		//线程数多了
		if(cur>=alive-busy||(double)busy/alive*100>=70&&alive+pmin<=pmax){
			
			pthread_mutex_lock(&ptr->lock);
			add=0;
			for(i=0;add<pmin&&i<pmax;i++){
				
				if(!ptr->ctid[i]||!is_pthread_alive(ptr->ctid[i])){
					pthread_create(&ptr->ctid[i],NULL,customer,(void*)ptr);
					ptr->pthread_alive++;
					add++;
					printf("add:%d\n",add);
					pthread_mutex_unlock(&ptr->lock);
				}	
				
			}	
			
		}
		//线程数少了
		if(busy*2<=alive-busy&&alive-pmin>=pmin){
			pthread_mutex_lock(&ptr->lock);
			for(int k=0;k<pmin;k++){
					ptr->deadcode = pmin;
					pthread_cond_signal(&ptr->Not_empty);
			}
			pthread_mutex_unlock(&ptr->lock);
		}
		printf("thread:alive:%d,busy:%d,cur:%d\n",ptr->pthread_alive,ptr->pthread_busy,ptr->cur);
		sleep(1);
	}
	return NULL;
}
