#include<server.h>

int epollcreate(){
	int epfd = epoll_create(EPOLEN);
	if(epfd>=0){
		return epfd;	
	}else{
		printf("epollcreate failed\n");
		return -1;
	}
}
