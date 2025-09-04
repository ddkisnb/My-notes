#include"INet.h"

class Udp : public INet {
public:
	Udp(INetMediator* p);
	~Udp();
	//初始化网络
	bool initNet();

	//发送数据(udp:ip ulong类型，决定发给谁；tcp：socket uint，决定了发给谁；）
	//unsigned long既能在udp里装ip，又能在tcp里装socket)
	bool sendData(char* data, int len, unsigned long to);

	//接收数据(放在线程里)
	void recvData();

	//关闭网络(回收线程资源、关闭套接字、卸载库)
	void unInitNet();

	//接收数据的线程
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	HANDLE m_handle;
};