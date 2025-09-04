#include"TcpClientMediator.h"
#include"../net/TcpClient.h"

TcpClientMediator::TcpClientMediator() {
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//������
bool TcpClientMediator::openNet() {
	return m_pNet->initNet();
}

//�ر�����
void TcpClientMediator::closeNet() {
	m_pNet->unInitNet();
}

//��������
bool TcpClientMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data, len, to);
}

//ת�����ݣ���net����յ������ݴ������Ĵ����ࣩ
//from�����Ǵ�������(udp:ip ulong���ͣ����������Ǵ������ģ�tcp��socket uint�����������Ǵ������ģ���
void TcpClientMediator::transmitData(char* data, int len, unsigned long from) {
	//TODO:�������Ĵ�����
	//���Դ��룺��ӡ���յ�������
	cout << "server say:" << data << ",len:" << len << endl;
}