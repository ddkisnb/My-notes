#include<server.h>

void*recv_business(void*arg){
	int sockfd = *(int*)arg;
	int recvlen;
	char buf[1024];
	char timebuf[1024];
	time_t tp;

	while((recvlen = recv(sockfd,buf,sizeof(buf),MSG_DONTWAIT))==-1){
		if(recvlen==EINTR){
			break;	
		}else{
			perror("recv failed\n");
			return NULL;
		}
	}

	if(recvlen==0){
		printf("client disconnect\n");
		close(sockfd);
		epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,NULL);
		return NULL;
	}

	if(strcmp(buf,"time\n")==0){
		tp = time(NULL);
		ctime_r(&tp,timebuf);
		send(sockfd,timebuf,strlen(timebuf),MSG_NOSIGNAL);
	}else{
		sprintf(timebuf,"notime\n");
		send(sockfd,timebuf,strlen(timebuf),MSG_NOSIGNAL);
	}
	return NULL;

}
