#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include"meetingdialog.h"
#include"TcpClientMediator.h"
#include"packdef.h"
#include"logindialog.h"
#include"roomdialog.h"
#include"audio_read.h"
#include"audio_write.h"
#include"videoread.h"
#include"threadworker.h"
#include"screenread.h"
#include"user.h"
#include"chat.h"
#include<QTextCodec>
#include<QByteArray>
class CKernel ;
class SendVideoWorker;
typedef void (CKernel ::*PFUN)(uint sock,char* buf,int nlen);
class CKernel : public QObject
{
    Q_OBJECT


public:
    explicit CKernel(QObject *parent = nullptr);
    explicit CKernel(const CKernel &kernel){}
    ~CKernel();
    static CKernel *GetInstance(){
        static CKernel kernel;
        return &kernel;
    }

signals:
    void  sig_Sendvideo(char*buf,int len);

public slots:

    //utf-8转到gb2312
    void utf8ToGb2312(QString src,char*dst,int len);
    //由gb2312转到utf-8
    QString gb2312ToUtf8(char*src);

    void setNetPackMap();
    void slot_destroy();
    void slot_dealData(uint sock,char* buf,int nlen);
    void slot_loginCommit(QString tel,QString password);
    void slot_registerCommit(QString name,QString tel,QString password);
    void slot_dealLoginRs(uint sock,char* buf,int nlen);
    void slot_dealRegisterRs(uint sock,char* buf,int nlen);

    void slot_createRoom();
    void slot_joinRoom();

    void slot_createRoomRS(uint sock,char* buf,int nlen);
    void slot_joinRoomRS(uint sock,char* buf,int nlen);
    void slot_RoomMember (uint sock,char* buf,int nlen);
    void slot_leaveRoomRq(uint sock,char* buf,int nlen);
    void slot_dealAudioFrameRq(uint sock,char* buf,int nlen);
    void slot_dealVideoFrameRq(uint sock,char* buf,int nlen);
    void slot_Sendvideo(char*buf,int len);
    void slot_quitRoom();

    void slot_AudioStart();
    void slot_AudioPause();
    void slot_VideoStart();
    void slot_VideoPause();

    void slot_ScreenStart();
    void slot_ScreenPause();


   void  slot_audioFrame(QByteArray ba);
   void slot_sendVideoFrame(QImage img);
    //刷新图片显示
   void slot_refreshVideo(int id,QImage &img);



   //处理添加好友请求
   void dealAddFriendRq(uint sock, char *buf, int nlen);
   //处理添加好友回复
   void dealAddFriendRs(uint sock, char *buf, int nlen);
   //处理自己以及好友的信息
   void dealInfo(uint sock, char *buf, int nlen);
   //处理聊天请求（B用户）
   void dealChatRq(uint sock, char *buf, int nlen);
   //处理聊天回复（A用户）
   void dealChatRs(uint sock, char *buf, int nlen);
   //显示与该好友的聊天窗口
   void slot_showChatDialog(int id);
   void slot_ChatInfo(int id,QString content);
   void slot_addFriend();


private:
    MeetingDialog *m_meetDialog;
    INetMediator* m_tcpClient;
    //音视频多条TCP发送，音频是0 ，视频是1
    enum client_type{audio_client =0, video_client };
    INetMediator* m_AVClient[2];

    loginDialog *m_login;
    RoomDialog *m_roomDialog;
    //协议映射表
    PFUN m_netPackMap[_DEF_PACK_COUNT];

    int  m_id;
    int  m_roomid;
    QString m_name;

    ///音频 ,一个采集，多个播放，每一个房间成员 1:1map映射
    Audio_Read  *m_AudioRead;
    std::map<int,Audio_Write*> m_mapIDtoAudioWrite;

    ///视频
    VideoRead *m_videoRead;
    QSharedPointer <SendVideoWorker>  m_psendVideo;
    ScreenRead *m_srceen;


    QMap<int,User*> m_mapIdToUseritem;
    QMap<int,chat*> m_mapIdToChatdlg;

};
class SendVideoWorker :public ThreadWorker{

     Q_OBJECT
public slots:
    void slot_sendvideo(char*buf,int len){
        CKernel::GetInstance()->slot_Sendvideo(buf,len);
    }
};

#endif // CKERNEL_H
