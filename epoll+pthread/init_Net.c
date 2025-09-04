#include<server.h>

int init_Net(){
	int sockfd;
	struct sockaddr_in addr;
	socklen_t addrlen;
	addrlen = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
		printf("socket failed\n");
		return -1;
	}

	if((bind(sockfd,(struct sockaddr*)&addr,addrlen))==-1){
		perror("bind failed\n");
		return -1;
	}

	listen(sockfd,128);
	printf("init Net success\n");
	return sockfd;
}
