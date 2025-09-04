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
	//��ʼ������
	virtual bool initNet() = 0;
	
	//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭��
	//unsigned long������udp��װip��������tcp��װsocket)
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//udp:sendto(socket,buf,len,flag,to,tolen);
	// tcp:send(socket,buf,len,flag)

	//��������(�����߳���)
	virtual void recvData() = 0;

	//�ر�����(�����߳���Դ���ر��׽��֡�ж�ؿ�)
	virtual void unInitNet() = 0;

protected:
	SOCKET m_sock;
	bool m_isRunning;
	INetMediator* m_pMediator;
};