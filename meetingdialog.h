#ifndef MEETINGDIALOG_H
#define MEETINGDIALOG_H

#include <QDialog>
#include<QCloseEvent>
#include<QAction>
#include<QMenu>
#include<QVBoxLayout>
#include"user.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MeetingDialog; }
QT_END_NAMESPACE

class MeetingDialog : public QDialog
{
    Q_OBJECT

signals:
        void sig_closeEvent();
        void sig_createRoom();
        void sig_joinRoom();
        void sig_addFriend();
public:
    MeetingDialog(QWidget *parent = nullptr);
    ~MeetingDialog();

    void closeEvent(QCloseEvent *event);

    void setInfo(int  id ,QString name, int icon);
    void addFriend(User*userIt);

private slots:
    void on_pb_createMeet_clicked();

    void on_pb_joinMeet_clicked();

    void on_pb_addFriend_clicked();

private:
    Ui::MeetingDialog *ui;
    int m_id;
    QString m_name;
    int iconId;
    QVBoxLayout *m_playout;

};
#endif // MEETINGDIALOG_H
