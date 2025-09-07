#include "user.h"
#include "ui_user.h"

User::User(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);
}

User::~User()
{
    delete ui;
}

void User::setFriendInfo(int id, QString name ,int iconId)
{
    m_id=id;
    m_name=name;
    //设置到界面上
    ui->lb_name->setText("我的好友:"+m_name);

}

const QString &User::name() const
{
    return m_name;
}



void User::on_pb_Icon_clicked()
{

    Q_EMIT sig_showChatDialog(m_id);
}

