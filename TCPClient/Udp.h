#include"INet.h"

class Udp : public INet {
public:
	Udp(INetMediator* p);
	~Udp();
	//��ʼ������
	bool initNet();

	//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭����
	//unsigned long������udp��װip��������tcp��װsocket)
	bool sendData(char* data, int len, unsigned long to);

	//��������(�����߳���)
	void recvData();

	//�ر�����(�����߳���Դ���ر��׽��֡�ж�ؿ�)
	void unInitNet();

	//�������ݵ��߳�
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	HANDLE m_handle;
};