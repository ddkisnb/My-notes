#include"TcpServer.h"
#include"../mediator/TcpServerMediator.h"

TcpServer::TcpServer(INetMediator* p) {
	m_pMediator = p;
}
TcpServer::~TcpServer() {}

//�������ݵ��̺߳���
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
	TcpServer* pThis = (TcpServer*)lpVoid;
	pThis->recvData();
	return 1;
}

//�������ӵ��̺߳���
unsigned __stdcall TcpServer::acceptThread(void* lpVoid) {
	cout << __func__ << endl;
	TcpServer* pThis = (TcpServer*)lpVoid;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in addr = {};
	int size = sizeof(addr);
	HANDLE handle = nullptr;
	unsigned int threadId = 0;
	while (pThis->m_isRunning) {
		//�������ӳɹ�ʱ���ص�socket���������ӵ�����ͻ��˽����շ�����
		sock = accept(pThis->m_sock, (sockaddr*)&addr, &size);
		if (INVALID_SOCKET == sock) {
			//��������ʧ���ˣ���ӡ������־������ѭ��
			cout << "accept error:" << WSAGetLastError() << endl;
			break;
		}
		else {
			//���ӳɹ�����ӡ�ͻ���ip
			cout << "client ip:" << inet_ntoa(addr.sin_addr) << endl;

			//������������ͻ������ݵ��߳�
			handle = (HANDLE)_beginthreadex(0/*�̰߳�ȫ����0��ʹ��Ĭ�ϵİ�ȫ����*/,
				0/*�̶߳�ջ��С��0��ʹ��Ĭ�϶�ջ��С1M*/,
				&recvThread/*�߳�Ҫִ�еĺ�������ʼ��ַ*/,
				pThis/*�߳�Ҫִ�еĺ����Ĳ���*/,
				0/*�̴߳�����״̬��0�Ǵ����Ժ����̿�ʼ���У�CREATE_SUSPENDED�Ǵ����Ժ����*/,
				&threadId/*�����߳��Ժ󣬲���ϵͳ��ÿ���̷߳�����߳�id*/);
			//�����߳̾��
			if (handle) {
				pThis->m_listHandle.push_back(handle);
			}
			//����socket
			pThis->m_mapThreadIdToSocket[threadId] = sock;
		}
	}
	return 1;
}

//��ʼ�����磺���ؿ⡢�����׽��֡���ip�Ͷ˿ڡ������������������ӵ��߳�
bool TcpServer::initNet() {
	//1�����ؿ�
	WORD version = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (0 != err) {
		cout << "WSAStrartup fail" << endl;
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

	//3����ip�Ͷ˿�
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_TCP_PORT);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "bind error:" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "bind success" << endl;
	}

	//4������
	err = listen(m_sock, _DEF_TCP_LISTEN_MAX);
	if (SOCKET_ERROR == err) {
		cout << "listen error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "listen success" << endl;
	}
	//5�������������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(0/*�̰߳�ȫ����0��ʹ��Ĭ�ϵİ�ȫ����*/,
		0/*�̶߳�ջ��С��0��ʹ��Ĭ�϶�ջ��С1M*/,
		&acceptThread/*�߳�Ҫִ�еĺ�������ʼ��ַ*/,
		this/*�߳�Ҫִ�еĺ����Ĳ���*/,
		0/*�̴߳�����״̬��0�Ǵ����Ժ����̿�ʼ���У�CREATE_SUSPENDED�Ǵ����Ժ����*/,
		nullptr/*�����߳��Ժ󣬲���ϵͳ��ÿ���̷߳�����߳�id*/);
	//�����߳̾��
	if (handle) {
		m_listHandle.push_back(handle);
	}
	return true;
}

//��������(udp:ip ulong���ͣ���������˭��tcp��socket uint�������˷���˭��
//unsigned long������udp��װip��������tcp��װsocket)
bool TcpServer::sendData(char* data, int len, unsigned long to) {
	cout << "TcpServer:" << __func__ << endl;
	//1��У������Ϸ���
	if (nullptr == data || len <= 0) {
		cout << "paramater error" << endl;
		return false;
	}

	//2���ȷ�������
	int nSendNum = send(to, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3���ٷ�������
	nSendNum = send(to, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

//��������(�����߳���)
void TcpServer::recvData() {
	cout << "TcpServer::" << __func__ << endl;
	//����һ�᣺�ȴ�acceptThread��socket���浽map�У���ȥmap��ȡֵ
	Sleep(5);
	//ȡ����ǰ�̵߳�socket�������socketȥ��������
	unsigned int threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadIdToSocket.count(threadId) > 0) {
		sock = m_mapThreadIdToSocket[threadId];
	}
	else {
		cout << "socket error" << endl;
		return;
	}

	//�������ݵĳ���
	int nRecvNum = 0;
	//������ĳ���
	int nPackLen = 0;
	//��¼һ�������ۼƽ��յ���������
	int nOffset = 0;
	while (m_isRunning) {
		//�Ƚ��հ�����
		nRecvNum = recv(sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0) {
			//���հ����ȳɹ��Ժ��ٽ��հ�����
			//���հ�����newһ���ռ�
			char* packBuf = new char[nPackLen];
			while (nPackLen > 0) {
				nRecvNum = recv(sock, packBuf + nOffset, nPackLen, 0);
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
			m_pMediator->transmitData(packBuf, nOffset, sock);
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
void TcpServer::unInitNet() {
	//1�������߳���Դ
	m_isRunning = false;
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		//ȡ����ǰ�ڵ��еľ��
		handle = *ite;
		//���վ��
		if (handle) {
			//WaitForSingleObject�ķ���ֵ�������WAIT_TIMEOUT����˵���ȴ����߳��ڵȴ�ʱ������󣬻��ڼ�������
			if (WAIT_TIMEOUT == WaitForSingleObject(handle/*�ȴ��ĸ��̣߳������ĸ��̵߳ľ��*/, 1000/*�ȴ���ʱ�䣬��λ�Ǻ���*/)) {
				//����̻߳��ڼ������У���Ҫǿ��ɱ���߳�
				TerminateThread(handle, -1);
			}
			//�رվ��
			CloseHandle(handle);
			handle = nullptr;
		}
		//��list���Ƴ���Ч�ڵ�(erase�Ƴ�������ָ��ĵ�ǰ�ڵ㣬������һ����Ч�ڵ�)
		ite = m_listHandle.erase(ite);
	}

	//2���ر��׽���
	if (!m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}
	SOCKET sock = INVALID_SOCKET;
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		//ȡ����ǰ�ڵ��е�socket
		sock = ite->second;
		//�ر��׽���
		if (!sock && INVALID_SOCKET != sock) {
			closesocket(sock);
		}
		//��map���Ƴ���Ч�ڵ�(erase�Ƴ�������ָ��ĵ�ǰ�ڵ㣬������һ����Ч�ڵ�)
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//3��ж�ؿ�
	WSACleanup();
}