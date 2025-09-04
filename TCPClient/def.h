#pragma once

//���ؿ�İ汾��
#define _DEF_VERSION_LOW  (2)
#define _DEF_VERSION_HIGH (2)
//UDPЭ��˿ں�
#define _DEF_UDP_PORT      (12345)
//TCPЭ��˿ں�
#define _DEF_TCP_PORT      (56789)
//TCPЭ��������е���󳤶�
#define _DEF_TCP_LISTEN_MAX (100)

#define _DEF_TCP_SERVER_IP  "127.0.0.1"

//�ǳơ��ֻ��š��������󳤶�
#define _DEF_MAX_LENTH      (15)
//�������ݳ���
#define _DEF_CONTENT_LENGTH (8*1024)

//�����ṹ������
#define _DEF_PROTOCOL_COUNT (10)

//�����ṹ�����ͺ�
#define _DEF_PROTOCOL_BASE  (1000)

//ע������
#define _DEF_REGISTER_RQ	(_DEF_PROTOCOL_BASE + 1)
//ע��ظ�
#define _DEF_REGISTER_RS	(_DEF_PROTOCOL_BASE + 2)
//��¼����
#define _DEF_LOGIN_RQ       (_DEF_PROTOCOL_BASE + 3)
//��½�ظ�
#define _DEF_LOGIN_RS		(_DEF_PROTOCOL_BASE + 4)
//��Ӻ�������
#define _DEF_ADD_FRIEND_RQ  (_DEF_PROTOCOL_BASE + 5)
//��Ӻ��ѻظ�
#define _DEF_ADD_FRIEND_RS  (_DEF_PROTOCOL_BASE + 6)
//��������
#define _DEF_CHAT_RQ		(_DEF_PROTOCOL_BASE + 7)
//����ظ�
#define _DEF_CHAT_RS		(_DEF_PROTOCOL_BASE + 8)
//��������
#define _DEF_OFFLINE_RQ		(_DEF_PROTOCOL_BASE + 9)
//�û���Ϣ
#define _DEF_FRIEND_INFO	(_DEF_PROTOCOL_BASE + 10)

//���������
//ע����
#define _def_register_success     (0)
#define _def_register_tel_exists  (1)
#define _def_register_name_exists (2)
//��½���
#define _def_login_success        (0)
#define _def_login_tel_not_exists (1)
#define _def_login_password_error (2)
//��Ӻ��ѵĽ��
#define _def_add_friend_success    (0)
#define _def_add_friend_offline     (1)
#define _def_add_friend_refuse     (2)
#define _def_add_friend_not_exists (3)
//���ͽ��
#define _def_send_success          (0)
#define _def_send_fail             (1)
//�û�״̬
#define _def_status_online         (0)
#define _def_status_offline        (1)

//�����ṹ�����ͱ���
typedef int packtype;

//����ṹ��
//ע�������ֻ��š����롢�ǳ�
typedef struct _STRU_REGISTER_RQ {
    _STRU_REGISTER_RQ() :type(_DEF_REGISTER_RQ) {
        memset(tel, 0, _DEF_MAX_LENTH);
        memset(password, 0, _DEF_MAX_LENTH);
        memset(name, 0, _DEF_MAX_LENTH);
    }
    packtype type;
    char tel[_DEF_MAX_LENTH];
    char password[_DEF_MAX_LENTH];
    char name[_DEF_MAX_LENTH];
}_STRU_REGISTER_RQ;

//ע��ظ���������ɹ����绰�����ѱ�ע�ᣬ�ǳ��ѱ�ע�ᣩ
typedef struct _STRU_REGISTER_RS {
    _STRU_REGISTER_RS() :result(_def_register_name_exists), type(_DEF_REGISTER_RS) {
    }
    packtype type;
    int result;
}_STRU_REGISTER_RS;

//��¼�����ֻ��š�����
typedef struct _STRU_LOGIN_RQ {
    _STRU_LOGIN_RQ() :type(_DEF_LOGIN_RQ) {
        memset(tel, 0, _DEF_MAX_LENTH);
        memset(password, 0, _DEF_MAX_LENTH);
    }
    packtype type;
    char tel[_DEF_MAX_LENTH];
    char password[_DEF_MAX_LENTH];
}_STRU_LOGIN_RQ;

//��½�ظ����������½�ɹ����绰��δע�ᣬ�������
typedef struct _STRU_LOGIN_RS {
    _STRU_LOGIN_RS() :result(_def_login_password_error), type(_DEF_LOGIN_RS), userId(0) {
    }
    packtype type;
    int userId;
    int result;
}_STRU_LOGIN_RS;

//��Ӻ������󣺺����ǳơ��Լ���id���Լ����ǳ�
typedef struct _STRU_ADD_FRIEND_RQ {
    _STRU_ADD_FRIEND_RQ() :userId(0), type(_DEF_ADD_FRIEND_RQ) {
        memset(userName, 0, _DEF_MAX_LENTH);
        memset(friendName, 0, _DEF_MAX_LENTH);
    }
    packtype type;
    int userId;
    char userName[_DEF_MAX_LENTH];
    char friendName[_DEF_MAX_LENTH];
}_STRU_ADD_FRIEND_RQ;

//��Ӻ��ѻظ����������ӳɹ������Ѳ����ڣ������Ѿܾ������Ѳ����ߣ����Լ���id���Լ����ǳƣ����ѵ�id�����ѵ��ǳ�
typedef struct _STRU_ADD_FRIEND_RS {
    _STRU_ADD_FRIEND_RS() :userId(0), friendId(0), result(_def_add_friend_not_exists), type(_DEF_ADD_FRIEND_RS) {
        memset(userName, 0, _DEF_MAX_LENTH);
        memset(freindName, 0, _DEF_MAX_LENTH);
    }
    packtype type;
    int result;
    int userId;
    int friendId;
    char userName[_DEF_MAX_LENTH];
    char freindName[_DEF_MAX_LENTH];
}_STRU_ADD_FRIEND_RS;

//���������������ݣ��Լ���id�����ѵ�id
typedef struct _STRU_CHAT_RQ {
    _STRU_CHAT_RQ() :userId(0), friendId(0), type(_DEF_CHAT_RQ) {
        memset(content, 0, _DEF_MAX_LENTH);
    }
    packtype type;
    char content[_DEF_CONTENT_LENGTH];
    int userId;
    int friendId;
}_STRU_CHAT_RQ;

//����ظ������ͽ�����ɹ���ʧ�ܣ�
typedef struct _STRU_CHAT_RS {
    _STRU_CHAT_RS() :result(_def_send_fail), type(_DEF_CHAT_RS), friendId(0) {
    }
    packtype type;
    int friendId;
    int result;
}_STRU_CHAT_RS;


//���������Լ���id
typedef struct _STRU_OFFLINE_RQ {
    _STRU_OFFLINE_RQ() :userId(0), type(_DEF_OFFLINE_RQ) {
    }
    packtype type;
    int userId;
}_STRU_OFFLINE_RQ;

//�û���Ϣ��type��id���ǳơ�ǩ����ͷ��id��״̬
typedef struct _STRU_FRIEND_INFO {
    _STRU_FRIEND_INFO() : type(_DEF_FRIEND_INFO),id(0),iconId(0),status(_def_status_offline){

    }
        packtype type;
    int id;
    int iconId;
    int status;
    char name[_DEF_MAX_LENTH];
    char feeling[_DEF_MAX_LENTH];
}_STRU_FRIEND_INFO;