#include<server.h>

void* accept_business(void*arg){
	int sockfd = *(int*)arg;
	int clientfd;
	struct sockaddr_in addr;
	struct epoll_event node;
	socklen_t addrlen;
	addrlen = sizeof(addr);

	printf("accept_business\n");
	if((clientfd=accept(sockfd,(struct sockaddr*)&addr,&addrlen))==-1){
		perror("accept failed\n");
		return NULL;
	}
	printf("connect clientfd:%d,ip:%s\n",clientfd,inet_ntoa(addr.sin_addr));

	node.data.fd = clientfd;
	node.events = EPOLLIN|EPOLLET;

	if(epoll_ctl(epfd,EPOLL_CTL_ADD,clientfd,&node)==-1){
		perror("epoll_ctl failed\n");
		return NULL;
	}

	//re
	char response[1500];
	char ip[16];
	bzero(response,sizeof(response));
	bzero(ip,sizeof(ip));
	inet_ntop(AF_INET,&addr.sin_addr.s_addr,ip,16);
	sprintf(response,"hi %s wellcome to epoll\n",ip);
	send(clientfd,response,strlen(response),MSG_NOSIGNAL);
	return NULL;
	
	
}
