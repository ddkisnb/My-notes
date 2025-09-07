#include "logindialog.h"
#include "ui_logindialog.h"
#include<QRegExp>
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("注册&登录");
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    Q_EMIT sig_close();
}

void loginDialog::on_pb_log_clicked()
{
    QString strTel=ui->le_phone_log->text();
    QString strPassword=ui->le_password_log->text();

    //1.手机号,正则表达式校验
    /*
            QRegExp reg("^1[3-8][0-9]\{6,9}$");
            bool res=reg.exactMatch(strTel);
*/
    Q_EMIT  sig_loginCommit(strTel,strPassword);
}

//注册
void loginDialog::on_pbreg_clicked()
{

    QString strTel=ui->le_phone_reg->text();
    QString strPassword=ui->le_password_reg->text();
    QString strName=ui->le_name_reg->text();

    Q_EMIT sig_RegCommit(strName,strTel,strPassword);

}

