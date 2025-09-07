#include "roomdialog.h"
#include "ui_roomdialog.h"

RoomDialog::RoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{
    ui->setupUi(this);
    m_mainLayout=new QVBoxLayout;
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(5);
    ui->wdg_list->setLayout(m_mainLayout);


}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::slot_setInfo(QString roomid)
{
    QString title =QString("欢迎来到昆总的VIP包房 ，您的房间号是:%1").arg(roomid);
    setWindowTitle(title);
    ui->lb_title->setText(title);
}

void RoomDialog::slot_addUser(UserShowForm *user)
{
    m_mainLayout->addWidget(user);
    map_IdToUserShow[user->m_id]=user;
}

void RoomDialog::slot_removeUser(UserShowForm *user)
{
    user->hide();
    m_mainLayout->removeWidget(user);
}

void RoomDialog::slot_refreshUser(int id, QImage &img)
{
    if(ui->wdg_userShow->m_id==id){
        ui->wdg_userShow->slot_setImage(img);
    }
    if(map_IdToUserShow.count(id)>0){

         UserShowForm *user=map_IdToUserShow[id];
         user->slot_setImage(img);
    }

}

void RoomDialog::slot_removeUser(int id)
{
    if(   map_IdToUserShow.count(id)>0){
        UserShowForm *user=map_IdToUserShow[id];
        slot_removeUser(user);
    }
}

void RoomDialog::slot_clearUser()
{

    for(auto ite =map_IdToUserShow.begin();
        ite!=map_IdToUserShow.end();++ite){
        slot_removeUser(ite->second);
    }
}

void RoomDialog::slot_setAudioCheck(bool check)
{
    ui->cb_auvio->setChecked(check);
}

void RoomDialog::slot_setVideoCheck(bool check)
{
     ui->cb_video->setChecked(check);
}

void RoomDialog::closeEvent(QCloseEvent *event)
{
   if( QMessageBox::question(this,"提示","，是否退出？")==
            QMessageBox::Yes){

       Q_EMIT sig_close();
       event->accept();
       return ;
   }
   event->ignore();
}

void RoomDialog::slot_setBigImgID(int id, QString name)
{
    ui->wdg_userShow->slot_SetInfo(id,name);

}



void RoomDialog::on_pb_exit_clicked()
{
    this->close();
}


void RoomDialog::on_cb_auvio_clicked()
{
    if(ui->cb_auvio->isChecked()){


        Q_EMIT SIG_AudioStart();



    }else{
             Q_EMIT SIG_AudioPause();

    }
}

//点击视频
void RoomDialog::on_cb_video_clicked()
{

    if(ui->cb_video->isChecked()){
        ui->cb_desk->setChecked(false);
        Q_EMIT SIG_ScreenPause();
        Q_EMIT SIG_VideoStart();

    }else{
             Q_EMIT SIG_VideoPause();

    }
}

//点击桌面
void RoomDialog::on_cb_desk_clicked()
{
    if(ui->cb_desk->isChecked()){
        ui->cb_video->setChecked(false);
        Q_EMIT SIG_VideoPause();
        Q_EMIT SIG_ScreenStart();

    }else{
             Q_EMIT SIG_ScreenPause();

    }
}


void RoomDialog::on_cb_moo_currentIndexChanged(int index)
{

   Q_EMIT  SIG_setMoji(index);

}

