#include"UdpMediator.h"
#include"../net/Udp.h"

UdpMediator::UdpMediator(){
	m_pNet = new Udp(this);
}
UdpMediator::~UdpMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//������
bool UdpMediator::openNet(){
	return m_pNet->initNet();
}

//�ر�����
void UdpMediator::closeNet() {
	m_pNet->unInitNet();
}

//��������
bool UdpMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data, len, to);
}

//ת�����ݣ���net����յ������ݴ������Ĵ����ࣩ
//from�����Ǵ�������(udp:ip ulong���ͣ����������Ǵ������ģ�tcp��socket uint�����������Ǵ������ģ���
void UdpMediator::transmitData(char* data, int len, unsigned long from) {
	//TODO:�������Ĵ�����
	//���Դ��룺��ӡ���յ�������
	cout << __func__ << ":" << data << endl;
}