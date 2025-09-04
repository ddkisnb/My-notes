#include"TcpClient.h"
#include"../mediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* p):m_handle(nullptr){}
TcpClient::~TcpClient(){}

//�������ݵ��̺߳��������ý������ݵĺ�����
unsigned __stdcall TcpClient::recvThread(void* lpVoid) {
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->recvData();
	return 1;
}

//��ʼ�����磺���ؿ⡢�����׽��֡����ӷ���ˡ�����һ���������ݵ��߳�
bool TcpClient::initNet() {
	//1�����ؿ�
	WORD version = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (0 != err) {
		cout << "WSAStartup fail" << endl;
		return false;
	}
	//�жϰ汾���Ƿ���ȷ
	if (_DEF_VERSION_HIGH == HIBYTE(data.wVersion) && _DEF_VERSION_LOW == LOBYTE(data.wVersion)) {
		cout << "WSAStartup success" << endl;
	}
	else {
		cout << "WSAStartup version error" << endl;
		return false;
	}

	//2�������׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error:" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket success" << endl;
	}

	//3�����ӷ����
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (SOCKET_ERROR == err) {
		cout << "connect error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "connect success" << endl;
	}

	//4������һ���������ݵ��߳�
	m_handle = (HANDLE)_beginthreadex(0/*�̰߳�ȫ����0��ʹ��Ĭ�ϵİ�ȫ����*/,
		0/*�̶߳�ջ��С��0��ʹ��Ĭ�϶�ջ��С1M*/,
		&recvThread/*�߳�Ҫִ�еĺ�������ʼ��ַ*/,
		this/*�߳�Ҫִ�еĺ����Ĳ���*/,
		0/*�̴߳�����״̬��0�Ǵ����Ժ����̿�ʼ���У�CREATE_SUSPENDED�Ǵ����Ժ����*/,
		nullptr/*�����߳��Ժ󣬲���ϵͳ��ÿ���̷߳�����߳�id*/);
	return true;
}

//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭��
//unsigned long������udp��װip��������tcp��װsocket)
bool TcpClient::sendData(char* data, int len, unsigned long to) {
	cout << "TcpClient:" << __func__ << endl;
	//1��У������Ϸ���
	if (nullptr == data || len <= 0) {
		cout << "paramater error" << endl;
		return false;
	}

	//2���ȷ�������
	int nSendNum = send(m_sock, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3���ٷ�������
	nSendNum = send(m_sock, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

//��������(�����߳���)
void TcpClient::recvData() {
	cout << "TcpClient::" << __func__ << endl;
	//�������ݵĳ���
	int nRecvNum = 0;
	//������ĳ���
	int nPackLen = 0;
	//��¼һ�������ۼƽ��յ���������
	int nOffset = 0;
	while (m_isRunning) {
		//�Ƚ��հ�����
		nRecvNum = recv(m_sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0) {
			//���հ����ȳɹ��Ժ��ٽ��հ�����
			//���հ�����newһ���ռ�
			char* packBuf = new char[nPackLen];
			while (nPackLen > 0) {
				nRecvNum = recv(m_sock, packBuf + nOffset, nPackLen, 0);
				if (nRecvNum > 0) {
					nOffset += nRecvNum;
					nPackLen -= nRecvNum;
				}
				else {
					cout << "recv error2:" << WSAGetLastError() << endl;
					break;
				}
			}
			//һ���������ݽ�����ɣ���ʱnPackLen = 0,nOffset��ɰ�����
			//TODO:�ѽ��յ������ݴ����н�����
			m_pMediator->transmitData(packBuf, nOffset, m_sock);
			//һ���������ݽ�����ɺ�nOffset����
			nOffset = 0;
		}
		else {
			cout << "recv error1:" << WSAGetLastError() << endl;
			break;
		}
	}
}

//�ر�����(�����߳���Դ���ر��׽��֡�ж�ؿ�)
void TcpClient::unInitNet() {
	//1�������߳���Դ
	//�����߳�ʱ������ϵͳ��ÿ���̷߳���3����Դ��������߳�ID���ں˶������ü�������2
	//��Ҫ�����߳���Դ������Ҫ�����ü��������0�������̹߳������رվ��
	m_isRunning = false;
	if (m_handle) {
		//WaitForSingleObject�ķ���ֵ�������WAIT_TIMEOUT����˵���ȴ����߳��ڵȴ�ʱ������󣬻��ڼ�������
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle/*�ȴ��ĸ��̣߳������ĸ��̵߳ľ��*/, 1000/*�ȴ���ʱ�䣬��λ�Ǻ���*/)) {
			//����̻߳��ڼ������У���Ҫǿ��ɱ���߳�
			TerminateThread(m_handle, -1);
		}
		//�رվ��
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//2���ر��׽���
	if (!m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}
	//3��ж�ؿ�
	WSACleanup();
}