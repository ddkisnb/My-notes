#include<server.h>

void epoll_listen(int sockfd,pool_t*ptr){
	struct epoll_event sock_array[EPOLEN];
	struct epoll_event node;
	node.data.fd = sockfd;
	node.events = EPOLLIN|EPOLLET;
	int ready,flag;
	task_t tmp;

	if(epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&node)==-1){
		perror("epoll-ctl failed\n");
		return;
	}
	
	printf("shut_down:%d\n",ptr->shut_down);
	while(!ptr->shut_down){
			if((ready = epoll_wait(epfd,sock_array,EPOLEN,-1))==-1){
				perror("epoll_wait failed\n");
				return;
			}
			printf("ready:%d\n",ready);
			flag=0;
			while(ready--){
				if(sock_array[flag].data.fd==sockfd){
					tmp.business = accept_business;
					tmp.arg =(void*)&sockfd;
					produce_add(tmp,ptr);
					printf("accept task\n");
				}else{
					tmp.business = recv_business;
					//tmp.business = test;
					tmp.arg = (void*)&(sock_array[flag].data.fd);
					produce_add(tmp,ptr);
				}
				flag++;
			}
	}

}
