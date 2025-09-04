#pragma once
#include"INetMediator.h"

class UdpMediator : public INetMediator {
public:
	UdpMediator();
	~UdpMediator();

	//打开网络
	bool openNet();

	//关闭网络
	void closeNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//转发数据（把net层接收到的数据传给核心处理类）
	//from数据是从哪来的(udp:ip ulong类型，决定数据是从哪来的；tcp：socket uint，决定数据是从哪来的；）
	void transmitData(char* data, int len, unsigned long from);
};