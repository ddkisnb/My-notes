#pragma once

//先声明有INet这个类，可以直接使用
class INet;
class INetMediator {
public:
	INetMediator():m_pNet(nullptr){}
	~INetMediator(){}

	//打开网络
	virtual bool openNet() = 0;

	//关闭网络
	virtual void closeNet() = 0;

	//发送数据
	virtual bool sendData(char* data, int len, unsigned long to) = 0;

	//转发数据（把net层接收到的数据传给核心处理类）
	//from数据是从哪来的(udp:ip ulong类型，决定数据是从哪来的；tcp：socket uint，决定数据是从哪来的；）
	virtual void transmitData(char* data, int len, unsigned long from) = 0;
protected:
	INet* m_pNet;
};