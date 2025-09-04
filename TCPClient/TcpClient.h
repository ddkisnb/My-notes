#include"INet.h"

class TcpClient : public INet {
public:
	TcpClient(INetMediator* p);
	~TcpClient();
	//��ʼ������
	bool initNet();

	//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭��
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