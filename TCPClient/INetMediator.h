#pragma once

//��������INet����࣬����ֱ��ʹ��
class INet;
class INetMediator {
public:
	INetMediator():m_pNet(nullptr){}
	~INetMediator(){}

	//������
	virtual bool openNet() = 0;

	//�ر�����
	virtual void closeNet() = 0;

	//��������
	virtual bool sendData(char* data, int len, unsigned long to) = 0;

	//ת�����ݣ���net����յ������ݴ������Ĵ����ࣩ
	//from�����Ǵ�������(udp:ip ulong���ͣ����������Ǵ������ģ�tcp��socket uint�����������Ǵ������ģ���
	virtual void transmitData(char* data, int len, unsigned long from) = 0;
protected:
	INet* m_pNet;
};