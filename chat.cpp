#include "chat.h"
#include "ui_chat.h"

chat::chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);
}

chat::~chat()
{
    delete ui;
}

void chat::setDialogInfo(int id, QString name)
{

    m_id=id;
    m_name=name;
    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_name));
}

void chat::setChatcontent(QString content)
{
    ui->tb_chat->append(QString("【%1】%2").arg(m_name).arg(QTime::currentTime().toString("hh:mm::ss")));
    ui->tb_chat->append(content);

}

void chat::on_pb_send_clicked()
{

    //1.获取用户输入的内容（纯文本）
    QString content =ui->te_chat->toPlainText();
    //2.校验用户输入的数据是否为空或者全是空格
    if(content.isEmpty()||content.remove("").isEmpty()){
        ui->te_chat->setText("");
        return ;
    }
    //3.获取用户输入的内容
    content=ui->te_chat->toHtml();
    qDebug()<<content;
    //4.设置到浏览窗口上
    ui->te_chat->setText("");
    ui->tb_chat->append(QString("【我】%1").arg(QTime::currentTime().toString("hh:mm::ss")));
    ui->tb_chat->append(content);
    //5.把聊天内容发给kernel
    Q_EMIT sig_ChatInfo(m_id,content);
}

