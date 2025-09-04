#include<server.h>

int main(){
	int sockfd = init_Net();
	epfd = epollcreate();
	printf("epfd:%d,sockfd:%d\n",epfd,sockfd);

	pool_t*ptr = createpool(1000,300,10);
	if(!ptr){
		printf("main:ptr not exit\n");	
	}
	epoll_listen(sockfd,ptr);
	return 0;
}
