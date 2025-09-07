#include "ckernel.h"
#include"QDebug"
#include<QMessageBox>
#define NetPackMap(a) m_netPackMap[a-DEF_PACK_BASE]
//添加协议映射表
void CKernel::setNetPackMap()
{
    memset(m_netPackMap,0,sizeof(m_netPackMap));//清空

    NetPackMap(DEF_PACK_LOGIN_RS)=&CKernel ::slot_dealLoginRs;
    NetPackMap(DEF_PACK_REGISTER_RS)=&CKernel ::slot_dealRegisterRs;
    NetPackMap(DEF_PACK_CREATEROOM_RS)=&CKernel ::slot_createRoomRS;
    NetPackMap(DEF_PACK_JOINROOM_RS)=&CKernel ::slot_joinRoomRS;
    NetPackMap(DEF_PACK_ROOM_MEMBER)=&CKernel ::slot_RoomMember;
    NetPackMap(DEF_PACK_LEAVEROOM_RQ)=&CKernel ::slot_leaveRoomRq;
    NetPackMap(DEF_PACK_AUDIO_FRAME)=&CKernel ::slot_dealAudioFrameRq;
    NetPackMap(DEF_PACK_VIDEO_FRAME)=&CKernel ::slot_dealVideoFrameRq;
    NetPackMap(_DEF_ADD_FRIEND_RQ)=&CKernel::dealAddFriendRq;
    NetPackMap(_DEF_ADD_FRIEND_RS)=&CKernel::dealAddFriendRs;
    NetPackMap(_DEF_CHAT_RQ)=&CKernel::dealChatRq;
    NetPackMap(_DEF_CHAT_RS)=&CKernel::dealChatRq;
    NetPackMap(_DEF_FRIEND_INFO)=&CKernel::dealInfo;




}
CKernel::CKernel(QObject *parent) : QObject(parent)
  ,m_id(0),m_roomid(0)
{
    setNetPackMap();
    m_meetDialog  =new MeetingDialog;
    connect(m_meetDialog ,SIGNAL(sig_closeEvent()),
            this,SLOT(slot_destroy()));

    connect(m_meetDialog ,SIGNAL(sig_createRoom()),
            this,SLOT(slot_createRoom()));
    connect(m_meetDialog ,SIGNAL(sig_joinRoom()),
            this,SLOT(slot_joinRoom()));

     m_login =new loginDialog;
     connect(m_login,SIGNAL(sig_loginCommit(QString,QString)),
             this,SLOT(slot_loginCommit(QString,QString)));
     connect(m_login,SIGNAL(sig_RegCommit(QString,QString,QString)),
             this,SLOT(slot_registerCommit(QString,QString,QString)));

     connect(m_login ,SIGNAL(sig_close())
             ,this,SLOT(slot_destroy()));
    m_login->show();



    m_tcpClient=new TcpClientMediator;
    m_tcpClient->OpenNet(_DEF_SERVER_IP,_DEF_TCP_PORT);
    connect(m_tcpClient,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealData(uint,char*,int)));
    for(int i=0; i <2; i ++){
           m_AVClient[i] = new TcpClientMediator;
           m_AVClient[i]->OpenNet(_DEF_SERVER_IP,_DEF_TCP_PORT);
           connect( m_AVClient[i],SIGNAL(SIG_ReadyData(uint,char*,int)),
                   this,SLOT(slot_dealData(uint,char*,int)));
    }




    m_roomDialog =new RoomDialog;
    connect(m_roomDialog,SIGNAL(sig_close()),
            this,SLOT(slot_quitRoom()));
    connect(m_roomDialog,SIGNAL(SIG_AudioStart()),
            this,SLOT(slot_AudioStart()));
    connect(m_roomDialog,SIGNAL(SIG_AudioPause()),
            this,SLOT(slot_AudioPause()));

    connect(m_roomDialog,SIGNAL(SIG_VideoStart()),
            this,SLOT(slot_VideoStart()));
    connect(m_roomDialog,SIGNAL(SIG_VideoPause()),
            this,SLOT(slot_VideoPause()));

    connect(m_roomDialog,SIGNAL(SIG_ScreenStart()),
            this,SLOT(slot_ScreenStart()));
    connect(m_roomDialog,SIGNAL(SIG_ScreenPause()),
            this,SLOT(slot_ScreenPause()));


    m_AudioRead =new Audio_Read;
    connect(m_AudioRead,SIGNAL(SIG_audioFrame(QByteArray)),
            this,SLOT(slot_audioFrame(QByteArray)));
    m_videoRead =new VideoRead;

    connect(m_videoRead,SIGNAL(SIG_sendVideoFrame(QImage)),
            this,SLOT(slot_sendVideoFrame(QImage)));

    m_psendVideo=QSharedPointer<SendVideoWorker> (new SendVideoWorker);
    connect(this,SIGNAL(sig_Sendvideo(char*,int)),
            m_psendVideo.data(),SLOT(slot_sendvideo(char*,int)));
    m_srceen =new ScreenRead;
    connect(m_srceen,SIGNAL(sig_SrceenRead(QImage)),
            this,SLOT(slot_sendVideoFrame(QImage)));

    //设置萌拍
     //connect(m_roomDialog,SIGNAL(SIG_setMoji(int)),m_videoRead,SLOT(slot_setLv(int)));

      connect(m_meetDialog,&MeetingDialog::sig_addFriend,this,&CKernel::slot_addFriend);

}

CKernel::~CKernel()
{

}



void CKernel::slot_destroy()
{
      qDebug()<<__func__;
    if(m_meetDialog){
        m_meetDialog->hide();
        delete m_meetDialog;
        m_meetDialog=nullptr;
    }
    if(m_tcpClient){
        delete m_tcpClient;
        m_tcpClient=nullptr;

    }
    if(m_login)
    {
        m_login->hide();
        delete m_login;
        m_login=nullptr;
    }
    if(m_AudioRead){
        m_AudioRead->pause();
        delete m_AudioRead;
        m_AudioRead=nullptr;
    }


}
//登录
void CKernel::slot_loginCommit(QString tel, QString password)
{

    std::string strTel =tel.toStdString();
    std::string strPass =password.toStdString();
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    strcpy(rq.password,strPass.c_str());
    m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));

}

void CKernel::slot_registerCommit(QString name, QString tel, QString password)
{
    qDebug()<<__func__;
    std::string strName= name.toStdString();
    std::string strTel =tel.toStdString();
    std::string strPass =password.toStdString();

    //名字处理：QString->std::string -->char*   兼容中文

    STRU_REGISTER_RQ rq;
    strcpy(rq.name,strName.c_str());
    strcpy(rq.tel,strTel.c_str());
    strcpy(rq.password,strPass.c_str());
    m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));
}

void CKernel::slot_dealData(uint sock, char *buf, int nlen)
{
    int type =*(int *)buf;
    if(type>=DEF_PACK_BASE && type<DEF_PACK_BASE +_DEF_PACK_COUNT){
    PFUN pf =NetPackMap(type);
    if(pf){
        (this->*pf)(sock,buf, nlen);
         }

    }
    delete[] buf;

}
//登录回复
void CKernel::slot_dealLoginRs(uint sock, char *buf, int nlen)
{

    qDebug()<<__func__;
    STRU_LOGIN_RS  *rs =(STRU_LOGIN_RS*)buf;
    switch(rs->result){

    case user_not_exist:
        QMessageBox::about(m_login, "提示","用户不存在");
        break;
    case password_error:
         QMessageBox::about(m_login, "提示","密码错误");
        break;
    case login_success:{

            m_name=QString::fromStdString(rs->name);
            m_meetDialog->setInfo(0,m_name,0);
            m_id=rs->userid;
            m_login->hide();
            m_meetDialog->showNormal();
            //注册和登录和 视频和音视频的FD
            STRU_AUDIO_FD rq_audio;
            rq_audio.user_id=m_id;
            STRU_VIDEO_FD rq_video;
            rq_video.user_id=m_id;

            m_AVClient[audio_client]->SendData(0,(char*)&rq_audio,sizeof(rq_audio));
            m_AVClient[video_client]->SendData(0,(char*)&rq_video,sizeof(rq_video));
    }
       break;
    }

}
//注册回复
void CKernel::slot_dealRegisterRs(uint sock, char *buf, int nlen)
{

    qDebug()<<__func__;
    STRU_REGISTER_RS  *rs =(STRU_REGISTER_RS*)buf;
    switch(rs->result){

    case tel_is_exist:
        QMessageBox::about(m_login, "提示","该手机号存在");
        break;
     case register_success:
         QMessageBox::about(m_login, "提示","注册成功！");
        break;
    case name_is_exist:
         QMessageBox::about(m_login, "提示","用户已经存在");
        break;

    default:
        break;
    }
}

#include<QInputDialog>
#include"QRegExp"
void CKernel::slot_createRoom()
{

     qDebug()<<__func__;
     if(m_roomid!=0){
         QMessageBox::about(m_meetDialog,"提示","在房间内，无法创建");
         return;
     }
     STUR_CREATE_ROOM_RQ rq;
     rq.m_userId=m_id;
     m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));
}

void CKernel::slot_joinRoom()
{
     qDebug()<<__func__;
    if(m_roomid!=0){
        QMessageBox::about(m_meetDialog,"提示","在房间内，无法加入");
                return;
    }
     QString strRoom=QInputDialog::getText(m_meetDialog,"加入房间","输入房间号");

     QRegExp exp("^[0-9]\{1,8}$");//0-9的数字，长度是1-10
     if(!exp.exactMatch(strRoom)){
         QMessageBox::about(m_meetDialog,"提示","房间号不合理,长度为不超过8的数字");
                 return;
     }


     STUR_JOIN_ROOM_RQ rq;
     rq.m_userId=m_id;
     rq.m_roomId=strRoom.toInt();
     m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));

}


void CKernel::slot_createRoomRS(uint sock, char *buf, int nlen)
{

     qDebug()<<__func__;
    STUR_CREATE_ROOM_RS *rs=(STUR_CREATE_ROOM_RS*)buf;
    m_roomDialog->slot_setInfo( QString::number(rs->roomId));

    UserShowForm *user =new UserShowForm;
    connect(user,SIGNAL(sig_itemCliecked(int,QString)),
            m_roomDialog,SLOT(slot_setBigImgID(int,QString)));
    user->slot_SetInfo(m_id,m_name);
    m_roomDialog->slot_addUser(user);
    m_roomid=rs->roomId;
    m_roomDialog->showNormal();

    m_roomDialog->slot_setAudioCheck(false);
    m_roomDialog->slot_setVideoCheck(false);
}

void CKernel::slot_joinRoomRS(uint sock, char *buf, int nlen)
{
      qDebug()<<__func__;
    STUR_JOIN_ROOM_RS *rs=(STUR_JOIN_ROOM_RS*)buf;
    if(rs->result==0){
        QMessageBox::about(m_meetDialog,"提示","房间号不存在，加入失败");
        return ;
    }
    m_roomDialog->slot_setInfo(QString::number(rs->roomId));

    m_roomid=rs->roomId;
    m_roomDialog->showNormal();
    m_roomDialog->slot_setAudioCheck(false);
    m_roomDialog->slot_setVideoCheck(false);
}

void CKernel::slot_RoomMember(uint sock, char *buf, int nlen)
{
    qDebug()<<__func__;
    STRU_ROOM_MEMBER_RQ*rq=(STRU_ROOM_MEMBER_RQ*)buf;
    UserShowForm*user=new  UserShowForm;

    user->slot_SetInfo(rq->user_id,QString::fromStdString(rq->m_szUser));
    connect(user,SIGNAL(sig_itemCliecked(int,QString)),
            m_roomDialog,SLOT(slot_setBigImgID(int,QString)));
    m_roomDialog->slot_addUser(user);

    Audio_Write *aw=NULL;
    if(m_mapIDtoAudioWrite.count(rq->user_id)==0)
    {
        aw=new Audio_Write;
        m_mapIDtoAudioWrite[rq->user_id]=aw;
    }

}

void CKernel::slot_leaveRoomRq(uint sock, char *buf, int nlen)
{
    STRU_LEAVEROOM_RQ *rq=(STRU_LEAVEROOM_RQ*)buf;
    if(rq->roomId==m_roomid ){
         m_roomDialog->slot_removeUser(rq->user_id);
    }
    if(m_mapIDtoAudioWrite.count(rq->user_id)>0)
    {
        Audio_Write*pAw=m_mapIDtoAudioWrite[rq->user_id];
        m_mapIDtoAudioWrite.erase(rq->user_id);
        delete pAw;
    }

}

//音频帧的处理
void CKernel::slot_dealAudioFrameRq(uint sock, char *buf, int nlen)
{
        //序列化
        char *tmp =buf;
        int userId;
        int roomId;
        tmp+=sizeof(int);

        userId=*(int *)tmp;
        tmp+=sizeof(int);

        roomId= *(int *)tmp;
        tmp+=sizeof(int);

        tmp+=sizeof(int);
        tmp+=sizeof(int);
        tmp+=sizeof(int);
        int nbufLen=nlen-6*sizeof(int);
        QByteArray ba(tmp,nbufLen);
        if(m_roomid==roomId){
            if(m_mapIDtoAudioWrite.count(userId)>0){
                Audio_Write*aw =m_mapIDtoAudioWrite[userId];
                aw->slot_net_rx(ba);
            }
        }

}

void CKernel::slot_dealVideoFrameRq(uint sock, char *buf, int nlen)
{

    //序列化
    int nbufLen=nlen-6*sizeof(int);
    char *tmp =buf;
    tmp+=sizeof(int);


    int userId=*(int *)tmp;
    tmp+=sizeof(int);

    int roomId=*(int *)tmp;
    tmp+=sizeof(int);

    tmp+=sizeof(int);
    tmp+=sizeof(int);
    tmp+=sizeof(int);

    QByteArray ba(tmp,nbufLen);
    QImage img;
    img.loadFromData(ba);

    if(m_roomid==roomId){
        m_roomDialog->slot_refreshUser(userId,img);
    }

}


void CKernel::slot_quitRoom()
{

       STRU_LEAVEROOM_RQ rq;
       rq.roomId=m_roomid;
       rq.user_id=m_id;
       std::string name=m_name.toStdString();
       strcpy(rq.m_szUser,name.c_str());

    m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));

    m_AudioRead->pause();
    m_videoRead->slot_closeVideo();
    m_roomDialog->slot_setAudioCheck(false);
    m_roomDialog->slot_setVideoCheck(false);
    //回收所有人的write
    for(auto ite =m_mapIDtoAudioWrite.begin();
        ite!=m_mapIDtoAudioWrite.end();){
        Audio_Write*pWrite =ite->second;
        ite=m_mapIDtoAudioWrite.erase(ite);
        delete pWrite;

    }
     m_roomDialog->slot_clearUser();
     m_roomid=0;


}

void CKernel::slot_AudioStart()
{
    m_AudioRead->start();
}

void CKernel::slot_AudioPause()
{
    m_AudioRead->pause();
}

void CKernel::slot_VideoStart()
{
    m_videoRead->slot_openVideo();
}

void CKernel::slot_VideoPause()
{
    m_videoRead->slot_closeVideo();
}

void CKernel::slot_ScreenStart()
{
    m_srceen->openVideo();

}

void CKernel::slot_ScreenPause()
{
    m_srceen->closeVideo();

}
#include<QTime>
//音频处理函数
void CKernel::slot_audioFrame(QByteArray ba)
{
    int nPacksize=6*sizeof(int)+ba.size();
    char *buf =new char[nPacksize];
    char *tmp =buf;
    //序列化
    int type =DEF_PACK_AUDIO_FRAME;
    int userId=m_id;
    int roomId=m_roomid;
    QTime tm=QTime ::currentTime();
    int min =tm.minute();
    int sec=tm.second();
    int msec=tm.msec();

    *(int *)tmp=type;
    tmp+=sizeof(int);

    *(int *)tmp=userId;
    tmp+=sizeof(int);

    *(int *)tmp=roomId;
    tmp+=sizeof(int);

    *(int *)tmp=min;
    tmp+=sizeof(int);

    *(int *)tmp=sec;
    tmp+=sizeof(int);

    *(int *)tmp=msec;
    tmp+=sizeof(int);

    memcpy(tmp,ba.data(),ba.size());

     m_AVClient[audio_client]->SendData(0,buf,nPacksize);
    delete []buf;



}
//视频处理函数
void CKernel::slot_Sendvideo(char *buf, int len)
{
    char *tmp =buf;
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    tmp+=sizeof(int);

    int min =*(int *)tmp;
    tmp+=sizeof(int);
    int sec =*(int *)tmp;
    tmp+=sizeof(int);
    int msec=*(int *)tmp;
    tmp+=sizeof(int);
//当前时间
    QTime ctm=QTime::currentTime();
    //数据包的时间
    QTime tm(ctm.hour(),min,sec,msec);
        if(tm.secsTo(ctm)>300){
            qDebug()<<"file"<<endl;
            delete [] buf;
            return;
        }
     m_AVClient[video_client]->SendData(0,buf,len);
     delete []buf;

}
#include<QBuffer>
void CKernel::slot_sendVideoFrame(QImage img)
{

    slot_refreshVideo(m_id,img);
    //显示//压缩
    QByteArray ba;
    QBuffer qbuf(&ba);
    img.save(&qbuf,"JPEG",50);

    int nPacksize=6*sizeof(int)+ba.size();
    char *buf =new char[nPacksize];
    char *tmp =buf;

    *(int *)tmp=DEF_PACK_VIDEO_FRAME;
    tmp+=sizeof(int);

    *(int *)tmp=m_id;
    tmp+=sizeof(int);

    *(int *)tmp=m_roomid;
    tmp+=sizeof(int);


    //延迟过久删除帧的参考时间
    QTime tm=QTime ::currentTime();
    *(int *)tmp=tm.minute();;
    tmp+=sizeof(int);

    *(int *)tmp=tm.second();;
    tmp+=sizeof(int);

    *(int *)tmp=tm.msec();;
    tmp+=sizeof(int);

    memcpy(tmp,ba.data(),ba.size());
    Q_EMIT sig_Sendvideo(buf,nPacksize );
    //写视频帧  发送
}

void CKernel::slot_refreshVideo(int id,QImage &img)
{
    m_roomDialog->slot_refreshUser(id,img);
}
//utf-8转到gb2312
void CKernel::utf8ToGb2312(QString src, char *dst, int len)
{
    QTextCodec*cd=QTextCodec::codecForName("gb2312");
    QByteArray ba= cd->fromUnicode(src);
    strcpy_s(dst,len,ba.data());
}
//由gb2312转到utf-8
QString CKernel::gb2312ToUtf8(char *src)
{
    QTextCodec*cd=QTextCodec::codecForName("gb2312");
    return  cd->toUnicode(src);
}

//处理添加好友请求(B用户)
void CKernel::dealAddFriendRq(uint sock, char *buf, int nlen)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RQ*rq=(_STRU_ADD_FRIEND_RQ*)buf;
    //2.弹出选择窗口
    _STRU_ADD_FRIEND_RS rs;
    QString str=QString("【%1】想添加你为好友，是否同意？").arg(rq->userName);
    if(QMessageBox::Yes==QMessageBox::question(m_meetDialog,"提示",str)){
        rs.result=_add_friend_success;
    }else{
        rs.result=_add_friend_refuse;
    }
    rs.userId=rq->userId;
    rs.friendId=m_id;
    strcpy_s(rs.friendName,sizeof(rs.friendName),rq->friendName);
    //3.结果发给服务端
    m_tcpClient->SendData(0,(char*)&rs,sizeof(rs));
}
//处理添加好友回复(A用户)
void CKernel::dealAddFriendRs(uint sock, char *buf, int nlen)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RS*rs=(_STRU_ADD_FRIEND_RS*)buf;
    QString friendName=gb2312ToUtf8(rs->friendName);
    //2.根据结果提示用户
    switch(rs->result){
    case _add_friend_success:
        QMessageBox::about(m_meetDialog,"提示",QString("添加【%1】为添加好友成功").arg(friendName));
        break;
    case _add_friend_not_exists:
          QMessageBox::about(m_meetDialog,"提示",QString("添加好友失败，【%1】不存在").arg(friendName));
        break;
    case _add_friend_refuse:
          QMessageBox::about(m_meetDialog,"提示",QString("【%1】拒绝添加你为好友").arg(friendName));
        break;
    default:
        break;
    }
}
//处理自己以及好友的信息
void CKernel::dealInfo(uint sock, char *buf, int nlen)
{
    qDebug() << __func__ << endl;
    //拆包
    _STRU_FRIEND_INFO* info=(_STRU_FRIEND_INFO*)buf;
    QString name= gb2312ToUtf8(info->friendName);

    //qDebug() <<"name"<<info->friendName;
    //2.判断是不是自己的信息
    if(info->friendId==m_id){
        //保存自己的信息
        m_name=name;
        //把自机的信息设置到界面上
        m_meetDialog->setInfo(info->friendId,name , info->iconId);
        return;
    }
    //3.好友的信息，判断是都已经在列表上了
    if(m_mapIdToUseritem.count(info->friendId)>0){
        //4.好友在列表上只需要更新好友的信息
        User *item =m_mapIdToUseritem[info->friendId];
        item->setFriendInfo(info->friendId,name,info->iconId);

    }else{
        //5.好友不在列表上，先new一个好友
        User*item=new User;

        //6.设置好友的信息
        item->setFriendInfo(info->friendId,name,info->iconId);
        //7.把好友添加到列表上
        m_meetDialog->addFriend(item);
        //绑定显示聊天窗口的信号和槽
        connect(item,&User::sig_showChatDialog,this,&CKernel::slot_showChatDialog);
        //8.保存useritem
        m_mapIdToUseritem[info->friendId]=item;
        //9.new一个跟这个好友的聊天窗口
        chat * c =new chat;
        //10.设置聊天窗口信息
        c->setDialogInfo(info->friendId,name);
        //11.把聊天窗口保存在map中
        m_mapIdToChatdlg[info->friendId]=c;
        //绑定发送聊天内容的信号和槽
         connect(c,&chat::sig_ChatInfo,this,&CKernel::slot_ChatInfo);
    }
}
////处理下线请求
//void CKernel::dealOfflineRq(uint sock, char *buf, int nlen)
//{
//     qDebug() << __func__ << endl;
//     //1.
//     _STRU_OFFLINE_RQ* rq=( _STRU_OFFLINE_RQ*)data;
//     //2.
//     //找到好友的useritem
//     if(m_mapIdToUseritem.count(rq->userId)>0){
//         //取出好友得useritem
//         useritem*item=m_mapIdToUseritem[rq->userId];
//         //修改好友得状态和头像
//         item->setFriendOffline();
//     }
//}
void CKernel::dealChatRq(uint sock, char *buf, int nlen)
{
    qDebug() << __func__ << endl;
    //1.拆包
    _STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)buf;
    //找到聊天窗口，把聊天窗口显示在窗口上
    if(m_mapIdToChatdlg.count(rq->userId)>0){
        chat *chat= m_mapIdToChatdlg[rq->userId];
        chat->setChatcontent(rq->content);
        chat->showNormal();
    }
}
void CKernel::dealChatRs(uint sock, char *buf, int nlen)
{
     qDebug() << __func__ << endl;
    //1.拆包
    _STRU_CHAT_RS* rs = (_STRU_CHAT_RS*)buf;
    //2.找到B聊天窗口，显示B用户不在线
    if(m_mapIdToChatdlg.count(rs->friendId)>0){
        chat *chat= m_mapIdToChatdlg[rs->friendId];
       chat->showNormal();

    }
 }

void CKernel::slot_showChatDialog(int id)
{
    qDebug()<<__func__;
    if(m_mapIdToChatdlg.count(id)>0){
        chat* chat=m_mapIdToChatdlg[id];
        chat->showNormal();
    }
}

void CKernel::slot_ChatInfo(int id, QString content)
{
     qDebug()<<__func__;
     //1.打包
     _STRU_CHAT_RQ rq;
     rq.userId=m_id;
     rq.friendId=id;
     strcpy_s(rq.content,sizeof(rq.content),content.toStdString().c_str());
     //2.发给服务端
      m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));

}
void CKernel::slot_addFriend()
{

     qDebug()<<__func__;
     //1.弹出询问窗口
     QString name=QInputDialog::getText(m_meetDialog,"添加好友","请输入好友的昵称:");
     QString nameTmp=name;
      qDebug()<<name;
      //2.判断用户输入的是否是合法
      if(name.isEmpty()||nameTmp.remove("").isEmpty()){
          QMessageBox::about(m_meetDialog,"提示","好友昵称不能为空或者是空格");
          return ;
      }

    //3.判断呢称是否是自己
      if(m_name==name){
             QMessageBox::about(m_meetDialog,"提示","好友不能是自己");
             return ;
      }
      //4.判断昵称是否已经加过好友了
      for(User* item:m_mapIdToUseritem){
          //取出好友的昵称
          QString friendName=item->name();
          if(friendName==name){
              QMessageBox::about(m_meetDialog,"提示","已经加过好友了,不要再加我了");
                return ;
          }
      }
      //5.给服务端发送加好友的请求
      _STRU_ADD_FRIEND_RQ rq;
      rq.userId=m_id;
      strcpy_s(rq.userName,sizeof(rq.userName),m_name.toStdString().c_str());
      utf8ToGb2312(name,rq.friendName,sizeof(rq.friendName));

         m_tcpClient->SendData(0,(char*)&rq,sizeof(rq));
}
