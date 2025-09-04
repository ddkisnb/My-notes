#include"TcpServer.h"
#include"../mediator/TcpServerMediator.h"

TcpServer::TcpServer(INetMediator* p) {
	m_pMediator = p;
}
TcpServer::~TcpServer() {}

//接收数据的线程函数
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
	TcpServer* pThis = (TcpServer*)lpVoid;
	pThis->recvData();
	return 1;
}

//接受连接的线程函数
unsigned __stdcall TcpServer::acceptThread(void* lpVoid) {
	cout << __func__ << endl;
	TcpServer* pThis = (TcpServer*)lpVoid;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in addr = {};
	int size = sizeof(addr);
	HANDLE handle = nullptr;
	unsigned int threadId = 0;
	while (pThis->m_isRunning) {
		//接受连接成功时返回的socket用来跟连接的这个客户端进行收发数据
		sock = accept(pThis->m_sock, (sockaddr*)&addr, &size);
		if (INVALID_SOCKET == sock) {
			//接受连接失败了，打印错误日志，跳出循环
			cout << "accept error:" << WSAGetLastError() << endl;
			break;
		}
		else {
			//连接成功，打印客户端ip
			cout << "client ip:" << inet_ntoa(addr.sin_addr) << endl;

			//创建接收这个客户端数据的线程
			handle = (HANDLE)_beginthreadex(0/*线程安全级别，0是使用默认的安全级别*/,
				0/*线程堆栈大小，0是使用默认堆栈大小1M*/,
				&recvThread/*线程要执行的函数的起始地址*/,
				pThis/*线程要执行的函数的参数*/,
				0/*线程创建的状态，0是创建以后立刻开始运行，CREATE_SUSPENDED是创建以后挂起*/,
				&threadId/*创建线程以后，操作系统给每个线程分配的线程id*/);
			//保存线程句柄
			if (handle) {
				pThis->m_listHandle.push_back(handle);
			}
			//保存socket
			pThis->m_mapThreadIdToSocket[threadId] = sock;
		}
	}
	return 1;
}

//初始化网络：加载库、创建套接字、绑定ip和端口、监听、创建接受连接的线程
bool TcpServer::initNet() {
	//1、加载库
	WORD version = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (0 != err) {
		cout << "WSAStrartup fail" << endl;
		return false;
	}
	//判断版本号是否正确
	if (_DEF_VERSION_HIGH == HIBYTE(data.wVersion) && _DEF_VERSION_LOW == LOBYTE(data.wVersion)) {
		cout << "WSAStartup success" << endl;
	}
	else {
		cout << "WSAStartup version error" << endl;
		return false;
	}

	//2、创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error:" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket success" << endl;
	}

	//3、绑定ip和端口
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

	//4、监听
	err = listen(m_sock, _DEF_TCP_LISTEN_MAX);
	if (SOCKET_ERROR == err) {
		cout << "listen error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "listen success" << endl;
	}
	//5、创建接受连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(0/*线程安全级别，0是使用默认的安全级别*/,
		0/*线程堆栈大小，0是使用默认堆栈大小1M*/,
		&acceptThread/*线程要执行的函数的起始地址*/,
		this/*线程要执行的函数的参数*/,
		0/*线程创建的状态，0是创建以后立刻开始运行，CREATE_SUSPENDED是创建以后挂起*/,
		nullptr/*创建线程以后，操作系统给每个线程分配的线程id*/);
	//保存线程句柄
	if (handle) {
		m_listHandle.push_back(handle);
	}
	return true;
}

//发送数据(udp:ip ulong类型，决定发给谁；tcp：socket uint，决定了发给谁；
//unsigned long既能在udp里装ip，又能在tcp里装socket)
bool TcpServer::sendData(char* data, int len, unsigned long to) {
	cout << "TcpServer:" << __func__ << endl;
	//1、校验参数合法性
	if (nullptr == data || len <= 0) {
		cout << "paramater error" << endl;
		return false;
	}

	//2、先发包长度
	int nSendNum = send(to, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3、再发包内容
	nSendNum = send(to, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

//接收数据(放在线程里)
void TcpServer::recvData() {
	cout << "TcpServer::" << __func__ << endl;
	//休眠一会：等待acceptThread把socket保存到map中，再去map中取值
	Sleep(5);
	//取出当前线程的socket，用这个socket去接收数据
	unsigned int threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadIdToSocket.count(threadId) > 0) {
		sock = m_mapThreadIdToSocket[threadId];
	}
	else {
		cout << "socket error" << endl;
		return;
	}

	//接收数据的长度
	int nRecvNum = 0;
	//保存包的长度
	int nPackLen = 0;
	//记录一个包中累计接收到多少数据
	int nOffset = 0;
	while (m_isRunning) {
		//先接收包长度
		nRecvNum = recv(sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0) {
			//接收包长度成功以后，再接收包内容
			//按照包长度new一个空间
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
			//一个包的内容接收完成，此时nPackLen = 0,nOffset变成包长度
			//TODO:把接收到的数据传给中介者类
			m_pMediator->transmitData(packBuf, nOffset, sock);
			//一个包的数据接收完成后，nOffset清零
			nOffset = 0;
		}
		else {
			cout << "recv error1:" << WSAGetLastError() << endl;
			break;
		}
	}
}

//关闭网络(回收线程资源、关闭套接字、卸载库)
void TcpServer::unInitNet() {
	//1、回收线程资源
	m_isRunning = false;
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		//取出当前节点中的句柄
		handle = *ite;
		//回收句柄
		if (handle) {
			//WaitForSingleObject的返回值如果等于WAIT_TIMEOUT，就说明等待的线程在等待时间结束后，还在继续运行
			if (WAIT_TIMEOUT == WaitForSingleObject(handle/*等待哪个线程，就填哪个线程的句柄*/, 1000/*等待的时间，单位是毫秒*/)) {
				//如果线程还在继续运行，就要强制杀死线程
				TerminateThread(handle, -1);
			}
			//关闭句柄
			CloseHandle(handle);
			handle = nullptr;
		}
		//从list中移除无效节点(erase移除迭代器指向的当前节点，返回下一个有效节点)
		ite = m_listHandle.erase(ite);
	}

	//2、关闭套接字
	if (!m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}
	SOCKET sock = INVALID_SOCKET;
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		//取出当前节点中的socket
		sock = ite->second;
		//关闭套接字
		if (!sock && INVALID_SOCKET != sock) {
			closesocket(sock);
		}
		//从map中移除无效节点(erase移除迭代器指向的当前节点，返回下一个有效节点)
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//3、卸载库
	WSACleanup();
}