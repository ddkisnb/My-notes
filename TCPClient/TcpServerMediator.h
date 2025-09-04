#pragma once
#include"INetMediator.h"

class TcpServerMediator : public INetMediator {
public:
	TcpServerMediator();
	~TcpServerMediator();

	//������
	bool openNet();

	//�ر�����
	void closeNet();

	//��������
	bool sendData(char* data, int len, unsigned long to);

	//ת�����ݣ���net����յ������ݴ������Ĵ����ࣩ
	//from�����Ǵ�������(udp:ip ulong���ͣ����������Ǵ������ģ�tcp��socket uint�����������Ǵ������ģ���
	void transmitData(char* data, int len, unsigned long from);
};