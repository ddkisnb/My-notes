#pragma once
#include<map>
#include<iostream>
#include"net/def.h"
#include"./MySQL/CMySql.h"
#include"mediator/INetMediator.h"

using namespace std;

//����ָ��������ָ��
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//������ָ�������ʼ����������
	void setProtocol();
	
	//�򿪷������������硢�������ݿ⣩
	bool startServer();
	//�رշ�������������Դ���ر����硢�Ͽ������ݿ�����ӣ�
	void endServer();

	//��ȡ��ǰ��½�û�����Ϣ�Լ����ѵ���Ϣ
	void getUserInfoFriendInfo(int userId);

	//����id��ѯ�û���Ϣ
	void getInfoById(int id, _STRU_FRIEND_INFO* info/*�鵽���û���Ϣ��Ϊ����������ظ����ú���*/);

	//�������н��յ�������
	void dealData(char* data, int len, unsigned long from);

	//����ע������
	void dealRegisterRq(char* data, int len, unsigned long from);

	//�����¼����
	void dealLoginRq(char* data, int len, unsigned long from);

	//������������
	void dealChatRq(char* data, int len, unsigned long from);

	//������������
	void dealOfflineRq(char* data, int len, unsigned long from);

	//������Ӻ�������
	void dealAddFriendRq(char* data, int len, unsigned long from);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* data, int len, unsigned long from);

	static CKernel* pKernel;
private:
	INetMediator* m_pMediator;
	CMySql m_sql;
	//��������ָ������
	PFUN m_protocol[_DEF_PROTOCOL_COUNT];
	//�����½�ɹ����û���socket���û�����ʱҪɾ����Ӧ��socket��
	map<int, SOCKET> m_mapIdToSocket;
};

