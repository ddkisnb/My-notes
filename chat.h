#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include<QTime>
#include<QDebug>
namespace Ui {
class chat;
}

class chat : public QDialog
{
    Q_OBJECT


signals:
    //把聊天内容发给kernel
    void sig_ChatInfo(int id,QString content);

public:
    explicit chat(QWidget *parent = nullptr);
    ~chat();
     //设置聊天窗口背景
     void setDialogInfo(int id,QString name);
     //设置好友的聊天内容
    void setChatcontent(QString content);

    private slots:
        void on_pb_send_clicked();
private:
    Ui::chat *ui;
    int m_id;
    QString m_name;
};

#endif // CHAT_H
