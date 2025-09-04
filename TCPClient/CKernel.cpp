#include "CKernel.h"
#include"mediator/TcpServerMediator.h"

CKernel* CKernel::pKernel = nullptr;

CKernel::CKernel():m_pMediator(nullptr){
	pKernel = this;
	setProtocol();
}
CKernel::~CKernel(){}

//给函数指针数组初始化并存数据
void CKernel::setProtocol() {
	cout << __func__ << endl;
	//初始化成0
	memset(m_protocol, 0, sizeof(m_protocol));
	//存入数据
	m_protocol[_DEF_REGISTER_RQ   - _DEF_PROTOCOL_BASE -1] = &CKernel::dealRegisterRq;
	m_protocol[_DEF_LOGIN_RQ      - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_protocol[_DEF_CHAT_RQ       - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_protocol[_DEF_OFFLINE_RQ    - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_protocol[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_protocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}

//打开服务器（打开网络、连接数据库）
bool CKernel::startServer() {
	//1、打开网络
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->openNet()) {
		cout << "打开服务端网络失败" << endl;
		return false;
	}

	//2、连接数据库
	char ip[] = "127.0.0.1";
	char user[] = "root";
	char pass[] = "colin123";
	char db[] = "im";
	if (!m_sql.ConnectMySql(ip, user, pass, db)) {
		cout << "连接数据库失败" << endl;
		return false;
	}

	return true;
}

//关闭服务器（回收资源、关闭网络、断开与数据库的连接）
void CKernel::endServer() {
	if (m_pMediator) {
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//处理所有接收到的数据
void CKernel::dealData(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//取出协议头
	packtype type = *(packtype*)data;
	
	//计算数组下标
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//判断下标是否在有效范围内
	if (index >= 0 && index < _DEF_PROTOCOL_COUNT) {
		//根据数组下标，取出函数地址
		PFUN pf = m_protocol[index];
		if (pf) {
			//调用具体的处理函数
			(this->*pf)(data, len, from);
		}
		else {
			//打印type2：1、定义结构体的时候，type值错了；2、对端发送的结构体不对
			cout << "type2:" << type << endl;
		}
	}
	else {
		//打印type1：1、声明结构体的时候packtype没有第一个声明；2、接收数据的时候nOffset没清零
		cout << "type1:" << type << endl;
	}

	//回收空间
	delete[] data;
}

//处理注册请求
void CKernel::dealRegisterRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	
	//2、校验电话号码是否被注册
	//从数据库中，根据电话号码查询电话号码
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select tel from t_user where tel = '%s';", rq->tel);
	//printf("select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/, 
		1/*sql语句中查询的列的个数*/, 
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}

	//判断查询结果是否为空
	if (0 != lstStr.size()) {
		//查询结果不为空，说明电话号码被注册过，注册失败_def_register_tel_exists
		_STRU_REGISTER_RS rs;
		rs.result = _def_register_tel_exists;
		
		//给客户端回复注册结果
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//结束
		return;
	}

	//3、校验昵称是否被注册
	//从数据库中，根据昵称查询昵称
	sprintf_s(sql, "select name from t_user where name = '%s';", rq->name);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		1/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}
	//判断查询结果是否为空
	if (0 != lstStr.size()) {
		//查询结果不为空，说明电话号码被注册过，注册失败_def_register_name_exists
		_STRU_REGISTER_RS rs;
		rs.result = _def_register_name_exists;

		//给客户端回复注册结果
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//结束
		return;
	}

	//4、把注册数据插入数据库
	sprintf_s(sql, "insert into t_user (name, tel, password,feeling, iconid) values ('%s','%s','%s','学习好累',8);",
		rq->name, rq->tel, rq->password);
	if (!m_sql.UpdateMySql(sql)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "插入数据库失败：" << sql << endl;
		return;
	}

	//5、注册成功
	_STRU_REGISTER_RS rs;
	rs.result = _def_register_success;

	//6、给客户端回复注册结果
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//处理登录请求
void CKernel::dealLoginRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2、从数据库查询，根据电话号码查询密码
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select password,id from t_user where tel = '%s';", rq->tel);
	//printf("select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		2/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}

	//3、判断查询结果是否为空
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size()) {
		//4、查询结果为空，说明电话号码未注册，登陆失败
		rs.result = _def_login_tel_not_exists;
	}
	else {
		//5、比较查询到的密码和输入的密码
		string pass = lstStr.front();
		lstStr.pop_front();

		int userId = stoi(lstStr.front());
		lstStr.pop_front();

		if (pass == rq->password) {
			//相等，就是登陆成功
			rs.result = _def_login_success;
			rs.userId = userId;

			//保存当前用户的的socket
			m_mapIdToSocket[userId] = from;

			//6、把结果发回给客户端
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);

			//获取当前登陆用户的信息以及好友的信息
			getUserInfoFriendInfo(userId);
			return;
		}
		else {
			//不相等，登陆失败，密码错误
			rs.result = _def_login_password_error;
		}
	}
	//6、把结果发回给客户端
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//获取当前登陆用户的信息以及好友的信息
void CKernel::getUserInfoFriendInfo(int userId) {
	cout << __func__ << endl;
	//根据自己的id查询自己的信息
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);

	//把自己的信息发回给客户端
	if (m_mapIdToSocket.count(userId) > 0) {
		m_pMediator->sendData((char*) & userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}
	else {
		cout << "m_mapIdToSocket中没有id：" << userId << endl;
		return;
	}

	//根据自己的id查询好友的id列表
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", userId);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		1/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}

	//遍历好友的id列表
	int friendId = 0; 
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size() > 0) {
		//取出每个好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//根据好友的id查询好友的信息
		getInfoById(friendId, &friendInfo);

		//把好友的信息发回给客户端
		if (m_mapIdToSocket.count(userId) > 0) {
			m_pMediator->sendData((char*)&friendInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
		}
		else {
			cout << "m_mapIdToSocket中没有id：" << userId << endl;
			return;
		}

		//判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//如果在线，通知好友自己上线了
			m_pMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

//根据id查询用户信息
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info/*查到的用户信息作为输出参数返回给调用函数*/) {
	cout << __func__ << endl;
	info->id = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//在线
		info->status = _def_status_online;
	}
	else {
		//不在线
		info->status = _def_status_offline;
	}

	//从数据库查询昵称、签名和头像id
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select name,feeling,iconid from t_user where id = %d;", id);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		3/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}

	if (3 == lstStr.size()) {
		//取出昵称
		strcpy_s(info->name, sizeof(info->name), lstStr.front().c_str());
		lstStr.pop_front();

		//取出签名
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();

		//取出头像id
		info->iconId = stoi(lstStr.front());
		lstStr.pop_front();
	}
	else {
		cout << "查询昵称、签名和头像id错误" << sql << endl;
	}
}

//处理聊天请求
void CKernel::dealChatRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2、判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//如果好友在线，把聊天请求转发给好友
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//好友不在线，给客户端回复发送失败
		//正常软件，应该给不在线好友发送的消息存入数据库里，等到好友上线的时候
		//再把数据库里面的聊天内容转发给好友，把数据库中保存的数据删掉
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}

//处理下线请求
void CKernel::dealOfflineRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2、查询好友id列表
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", rq->userId);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		1/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}

	//3、遍历好友id列表
	int friendId = 0;
	while (lstStr.size() > 0) {
		//4、取出好友id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5、判断好友是否在线，在线就转发下线请求给好友
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}
	//6、关闭下线用户的socket，并且回收map的空间
	auto ite = m_mapIdToSocket.find(rq->userId);
	if (ite != m_mapIdToSocket.end()) {
		closesocket(ite->second);
		m_mapIdToSocket.erase(ite);
	}
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2、根据好友昵称查询好友id
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		1/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/)) {
		//只有两个原因：1、没有连接数据库；
		//2、sql语句有语法错误，或者列名和表对应不上(把日志中打印的sql语句拷贝到workbanch里面运行)
		cout << "查询数据库失败：" << sql << endl;
		return;
	}
	//todo
	cout << "添加好友查询成功" << endl;
	//3、判断查询结果是否为空
	if (0 == lstStr.size()) {
		//4、查询结果为空，说明昵称不存在，添加失败，_def_add_friend_not_exists
		_STRU_ADD_FRIEND_RS rs;
		rs.result = _def_add_friend_not_exists;
		strcpy_s(rs.freindName, sizeof(rs.freindName), rq->friendName);
		//todo
		cout << "添加失败" << endl;
		//把添加结果发回给A客户端
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//取出好友的id
		int friendId = stoi(lstStr.front());
		lstStr.pop_front();
		
		//5、判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//6、好友在线，把添加好友的请求转发给好友B客户端
			cout << "好友在线" << endl;
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//7、好友不在线，添加失败
			_STRU_ADD_FRIEND_RS rs;
			rs.result = _def_add_friend_offline;
			strcpy_s(rs.freindName, sizeof(rs.freindName), rq->friendName);
			cout << "好友不在线" << endl;
			//把添加结果发回给A客户端
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2、判断结果是否同意
	if (_def_add_friend_success == rs->result) {
		//3、如果同意，把好友关系写入数据库中
		char sql[1024] = "";
		sprintf_s(sql, "insert into t_friend values (%d, %d);", rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(sql)) {
			cout << "插入数据库失败" << sql << endl;
			return;
		}

		sprintf_s(sql, "insert into t_friend values (%d, %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sql)) {
			cout << "插入数据库失败" << sql << endl;
			return;
		}

		//4、更新双端好友列表
		getUserInfoFriendInfo(rs->friendId);
	}

	//5、不管结果如何，都要把结果转发给A客户端
	if (m_mapIdToSocket[rs->userId] > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
	}
}
