#include "CKernel.h"
#include"mediator/TcpServerMediator.h"

CKernel* CKernel::pKernel = nullptr;

CKernel::CKernel():m_pMediator(nullptr){
	pKernel = this;
	setProtocol();
}
CKernel::~CKernel(){}

//������ָ�������ʼ����������
void CKernel::setProtocol() {
	cout << __func__ << endl;
	//��ʼ����0
	memset(m_protocol, 0, sizeof(m_protocol));
	//��������
	m_protocol[_DEF_REGISTER_RQ   - _DEF_PROTOCOL_BASE -1] = &CKernel::dealRegisterRq;
	m_protocol[_DEF_LOGIN_RQ      - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_protocol[_DEF_CHAT_RQ       - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_protocol[_DEF_OFFLINE_RQ    - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_protocol[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_protocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}

//�򿪷������������硢�������ݿ⣩
bool CKernel::startServer() {
	//1��������
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->openNet()) {
		cout << "�򿪷��������ʧ��" << endl;
		return false;
	}

	//2���������ݿ�
	char ip[] = "127.0.0.1";
	char user[] = "root";
	char pass[] = "colin123";
	char db[] = "im";
	if (!m_sql.ConnectMySql(ip, user, pass, db)) {
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}

	return true;
}

//�رշ�������������Դ���ر����硢�Ͽ������ݿ�����ӣ�
void CKernel::endServer() {
	if (m_pMediator) {
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//�������н��յ�������
void CKernel::dealData(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//ȡ��Э��ͷ
	packtype type = *(packtype*)data;
	
	//���������±�
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//�ж��±��Ƿ�����Ч��Χ��
	if (index >= 0 && index < _DEF_PROTOCOL_COUNT) {
		//���������±꣬ȡ��������ַ
		PFUN pf = m_protocol[index];
		if (pf) {
			//���þ���Ĵ�����
			(this->*pf)(data, len, from);
		}
		else {
			//��ӡtype2��1������ṹ���ʱ��typeֵ���ˣ�2���Զ˷��͵Ľṹ�岻��
			cout << "type2:" << type << endl;
		}
	}
	else {
		//��ӡtype1��1�������ṹ���ʱ��packtypeû�е�һ��������2���������ݵ�ʱ��nOffsetû����
		cout << "type1:" << type << endl;
	}

	//���տռ�
	delete[] data;
}

//����ע������
void CKernel::dealRegisterRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	
	//2��У��绰�����Ƿ�ע��
	//�����ݿ��У����ݵ绰�����ѯ�绰����
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select tel from t_user where tel = '%s';", rq->tel);
	//printf("select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/, 
		1/*sql����в�ѯ���еĸ���*/, 
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	//�жϲ�ѯ����Ƿ�Ϊ��
	if (0 != lstStr.size()) {
		//��ѯ�����Ϊ�գ�˵���绰���뱻ע�����ע��ʧ��_def_register_tel_exists
		_STRU_REGISTER_RS rs;
		rs.result = _def_register_tel_exists;
		
		//���ͻ��˻ظ�ע����
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//����
		return;
	}

	//3��У���ǳ��Ƿ�ע��
	//�����ݿ��У������ǳƲ�ѯ�ǳ�
	sprintf_s(sql, "select name from t_user where name = '%s';", rq->name);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		1/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}
	//�жϲ�ѯ����Ƿ�Ϊ��
	if (0 != lstStr.size()) {
		//��ѯ�����Ϊ�գ�˵���绰���뱻ע�����ע��ʧ��_def_register_name_exists
		_STRU_REGISTER_RS rs;
		rs.result = _def_register_name_exists;

		//���ͻ��˻ظ�ע����
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//����
		return;
	}

	//4����ע�����ݲ������ݿ�
	sprintf_s(sql, "insert into t_user (name, tel, password,feeling, iconid) values ('%s','%s','%s','ѧϰ����',8);",
		rq->name, rq->tel, rq->password);
	if (!m_sql.UpdateMySql(sql)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "�������ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	//5��ע��ɹ�
	_STRU_REGISTER_RS rs;
	rs.result = _def_register_success;

	//6�����ͻ��˻ظ�ע����
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//�����¼����
void CKernel::dealLoginRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2�������ݿ��ѯ�����ݵ绰�����ѯ����
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select password,id from t_user where tel = '%s';", rq->tel);
	//printf("select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		2/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	//3���жϲ�ѯ����Ƿ�Ϊ��
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size()) {
		//4����ѯ���Ϊ�գ�˵���绰����δע�ᣬ��½ʧ��
		rs.result = _def_login_tel_not_exists;
	}
	else {
		//5���Ƚϲ�ѯ������������������
		string pass = lstStr.front();
		lstStr.pop_front();

		int userId = stoi(lstStr.front());
		lstStr.pop_front();

		if (pass == rq->password) {
			//��ȣ����ǵ�½�ɹ�
			rs.result = _def_login_success;
			rs.userId = userId;

			//���浱ǰ�û��ĵ�socket
			m_mapIdToSocket[userId] = from;

			//6���ѽ�����ظ��ͻ���
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);

			//��ȡ��ǰ��½�û�����Ϣ�Լ����ѵ���Ϣ
			getUserInfoFriendInfo(userId);
			return;
		}
		else {
			//����ȣ���½ʧ�ܣ��������
			rs.result = _def_login_password_error;
		}
	}
	//6���ѽ�����ظ��ͻ���
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//��ȡ��ǰ��½�û�����Ϣ�Լ����ѵ���Ϣ
void CKernel::getUserInfoFriendInfo(int userId) {
	cout << __func__ << endl;
	//�����Լ���id��ѯ�Լ�����Ϣ
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);

	//���Լ�����Ϣ���ظ��ͻ���
	if (m_mapIdToSocket.count(userId) > 0) {
		m_pMediator->sendData((char*) & userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}
	else {
		cout << "m_mapIdToSocket��û��id��" << userId << endl;
		return;
	}

	//�����Լ���id��ѯ���ѵ�id�б�
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", userId);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		1/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	//�������ѵ�id�б�
	int friendId = 0; 
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size() > 0) {
		//ȡ��ÿ�����ѵ�id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//���ݺ��ѵ�id��ѯ���ѵ���Ϣ
		getInfoById(friendId, &friendInfo);

		//�Ѻ��ѵ���Ϣ���ظ��ͻ���
		if (m_mapIdToSocket.count(userId) > 0) {
			m_pMediator->sendData((char*)&friendInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
		}
		else {
			cout << "m_mapIdToSocket��û��id��" << userId << endl;
			return;
		}

		//�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//������ߣ�֪ͨ�����Լ�������
			m_pMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

//����id��ѯ�û���Ϣ
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info/*�鵽���û���Ϣ��Ϊ����������ظ����ú���*/) {
	cout << __func__ << endl;
	info->id = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//����
		info->status = _def_status_online;
	}
	else {
		//������
		info->status = _def_status_offline;
	}

	//�����ݿ��ѯ�ǳơ�ǩ����ͷ��id
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select name,feeling,iconid from t_user where id = %d;", id);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		3/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	if (3 == lstStr.size()) {
		//ȡ���ǳ�
		strcpy_s(info->name, sizeof(info->name), lstStr.front().c_str());
		lstStr.pop_front();

		//ȡ��ǩ��
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();

		//ȡ��ͷ��id
		info->iconId = stoi(lstStr.front());
		lstStr.pop_front();
	}
	else {
		cout << "��ѯ�ǳơ�ǩ����ͷ��id����" << sql << endl;
	}
}

//������������
void CKernel::dealChatRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2���жϺ����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//����������ߣ�����������ת��������
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//���Ѳ����ߣ����ͻ��˻ظ�����ʧ��
		//���������Ӧ�ø������ߺ��ѷ��͵���Ϣ�������ݿ���ȵ��������ߵ�ʱ��
		//�ٰ����ݿ��������������ת�������ѣ������ݿ��б��������ɾ��
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}

//������������
void CKernel::dealOfflineRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2����ѯ����id�б�
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", rq->userId);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		1/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}

	//3����������id�б�
	int friendId = 0;
	while (lstStr.size() > 0) {
		//4��ȡ������id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5���жϺ����Ƿ����ߣ����߾�ת���������������
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}
	//6���ر������û���socket�����һ���map�Ŀռ�
	auto ite = m_mapIdToSocket.find(rq->userId);
	if (ite != m_mapIdToSocket.end()) {
		closesocket(ite->second);
		m_mapIdToSocket.erase(ite);
	}
}

//������Ӻ�������
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2�����ݺ����ǳƲ�ѯ����id
	list<string> lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(sql/*Ҫִ�е�sql���*/,
		1/*sql����в�ѯ���еĸ���*/,
		lstStr/*sql����ѯ���Ľ��*/)) {
		//ֻ������ԭ��1��û���������ݿ⣻
		//2��sql������﷨���󣬻��������ͱ��Ӧ����(����־�д�ӡ��sql��俽����workbanch��������)
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sql << endl;
		return;
	}
	//todo
	cout << "��Ӻ��Ѳ�ѯ�ɹ�" << endl;
	//3���жϲ�ѯ����Ƿ�Ϊ��
	if (0 == lstStr.size()) {
		//4����ѯ���Ϊ�գ�˵���ǳƲ����ڣ����ʧ�ܣ�_def_add_friend_not_exists
		_STRU_ADD_FRIEND_RS rs;
		rs.result = _def_add_friend_not_exists;
		strcpy_s(rs.freindName, sizeof(rs.freindName), rq->friendName);
		//todo
		cout << "���ʧ��" << endl;
		//����ӽ�����ظ�A�ͻ���
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//ȡ�����ѵ�id
		int friendId = stoi(lstStr.front());
		lstStr.pop_front();
		
		//5���жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//6���������ߣ�����Ӻ��ѵ�����ת��������B�ͻ���
			cout << "��������" << endl;
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//7�����Ѳ����ߣ����ʧ��
			_STRU_ADD_FRIEND_RS rs;
			rs.result = _def_add_friend_offline;
			strcpy_s(rs.freindName, sizeof(rs.freindName), rq->friendName);
			cout << "���Ѳ�����" << endl;
			//����ӽ�����ظ�A�ͻ���
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2���жϽ���Ƿ�ͬ��
	if (_def_add_friend_success == rs->result) {
		//3�����ͬ�⣬�Ѻ��ѹ�ϵд�����ݿ���
		char sql[1024] = "";
		sprintf_s(sql, "insert into t_friend values (%d, %d);", rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(sql)) {
			cout << "�������ݿ�ʧ��" << sql << endl;
			return;
		}

		sprintf_s(sql, "insert into t_friend values (%d, %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sql)) {
			cout << "�������ݿ�ʧ��" << sql << endl;
			return;
		}

		//4������˫�˺����б�
		getUserInfoFriendInfo(rs->friendId);
	}

	//5�����ܽ����Σ���Ҫ�ѽ��ת����A�ͻ���
	if (m_mapIdToSocket[rs->userId] > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
	}
}
