#ifndef USER_H
#define USER_H

#include <QWidget>
#include<QBitmap>
#include<QDebug>
#include"packdef.h"

namespace Ui {
class User;
}

class User : public QWidget
{
    Q_OBJECT

signals:
    //通知kernel显示好友聊天窗口
    void sig_showChatDialog(int id);

public:
    explicit User(QWidget *parent = nullptr);
    ~User();
    //设置好友信息
    void setFriendInfo(int id,QString name,int iconId);
    const QString &name() const;

private slots:


    void on_pb_Icon_clicked();

private:
    Ui::User *ui;
    int m_id;
    QString m_name;
    int m_iconId;

};

#endif // USER_H
