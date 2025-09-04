#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<iostream>
#include<process.h>
#include"def.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

class INetMediator;
class INet {
public:
	INet():m_sock(INVALID_SOCKET),m_isRunning(true), m_pMediator(nullptr){}
	~INet(){}
	//初始化网络
	virtual bool initNet() = 0;
	
	//发送数据(udp:ip ulong类型，决定发给谁；tcp：socket uint，决定了发给谁；
	//unsigned long既能在udp里装ip，又能在tcp里装socket)
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//udp:sendto(socket,buf,len,flag,to,tolen);
	// tcp:send(socket,buf,len,flag)

	//接收数据(放在线程里)
	virtual void recvData() = 0;

	//关闭网络(回收线程资源、关闭套接字、卸载库)
	virtual void unInitNet() = 0;

protected:
	SOCKET m_sock;
	bool m_isRunning;
	INetMediator* m_pMediator;
};