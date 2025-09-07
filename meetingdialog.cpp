#include "meetingdialog.h"
#include "ui_meetingdialog.h"
#include"QMessageBox"
#include"QDebug"
MeetingDialog::MeetingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MeetingDialog)
{
    ui->setupUi(this);
    setWindowTitle("开会啦");
    m_playout=new QVBoxLayout;
    m_playout->setContentsMargins(0,0,0,0);
    m_playout->setSpacing(3);

    ui->wdg_list->setLayout(m_playout);


}

MeetingDialog::~MeetingDialog()
{
    delete ui;
}

void MeetingDialog::closeEvent(QCloseEvent *event)
{

    if(QMessageBox::question(this,"提示","是否退出?")
            ==QMessageBox::Yes)
    {
        Q_EMIT sig_closeEvent();
       event->accept();
    }else{
        event->ignore();
    }
}
//设置和头像
void MeetingDialog::setInfo(int id ,QString name, int icon)
{
    m_id=id;
    m_name=name;
    iconId=icon;
    ui->lb_name->setText(m_name);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/11.png").arg(iconId)));


}

void MeetingDialog::addFriend(User *userIt)
{
    m_playout->addWidget(userIt);

}

//创建会议
void MeetingDialog::on_pb_createMeet_clicked()
{

    Q_EMIT sig_createRoom();
}

//加入会议
void MeetingDialog::on_pb_joinMeet_clicked()
{

    Q_EMIT sig_joinRoom();
}

void MeetingDialog::on_pb_addFriend_clicked()
{
    Q_EMIT sig_addFriend();
}



