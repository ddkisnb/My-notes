#include"TcpServerMediator.h"
#include"../net/TcpServer.h"
#include "../CKernel.h"

TcpServerMediator::TcpServerMediator(){
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator(){
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//打开网络
bool TcpServerMediator::openNet() {
	return m_pNet->initNet();
}

//关闭网络
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}

//发送数据
bool TcpServerMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data,len,to);
}

//转发数据（把net层接收到的数据传给核心处理类）
//from数据是从哪来的(udp:ip ulong类型，决定数据是从哪来的；tcp：socket uint，决定数据是从哪来的；）
void TcpServerMediator::transmitData(char* data, int len, unsigned long from) {
	//传给核心处理类
	CKernel::pKernel->dealData(data, len, from);
}