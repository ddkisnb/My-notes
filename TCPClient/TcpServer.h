#include"INet.h"
#include<list>
#include<map>

class TcpServer : public INet {
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//��ʼ������
	bool initNet();

	//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭��
	//unsigned long������udp��װip��������tcp��װsocket)
	bool sendData(char* data, int len, unsigned long to);

	//��������(�����߳���)
	void recvData();

	//�ر�����(�����߳���Դ���ر��׽��֡�ж�ؿ�)
	void unInitNet();

	//�������ӵ��̺߳���
	static unsigned __stdcall acceptThread(void* lpVoid);

	//�������ݵ��̺߳���
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	list<HANDLE> m_listHandle;
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;
};