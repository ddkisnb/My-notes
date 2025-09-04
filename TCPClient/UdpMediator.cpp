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

//打开网络
bool UdpMediator::openNet(){
	return m_pNet->initNet();
}

//关闭网络
void UdpMediator::closeNet() {
	m_pNet->unInitNet();
}

//发送数据
bool UdpMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data, len, to);
}

//转发数据（把net层接收到的数据传给核心处理类）
//from数据是从哪来的(udp:ip ulong类型，决定数据是从哪来的；tcp：socket uint，决定数据是从哪来的；）
void UdpMediator::transmitData(char* data, int len, unsigned long from) {
	//TODO:传给核心处理类
	//测试代码：打印接收到的数据
	cout << __func__ << ":" << data << endl;
}