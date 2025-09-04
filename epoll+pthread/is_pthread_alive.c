#include<server.h>

int is_pthread_alive(pthread_t tid){
	pthread_kill(tid,0);
	if(errno==ESRCH){
		return 0;	
	}else{
		return 1;	
	}
}
