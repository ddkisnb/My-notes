#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
#include<QVBoxLayout>
#include"usershowform.h"
#include<QCloseEvent>
#include<QMessageBox>
namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void sig_close();
    void SIG_AudioPause();
    void SIG_AudioStart();
    void SIG_VideoStart();
    void SIG_VideoPause();
    void SIG_ScreenStart();
    void SIG_ScreenPause();
    void SIG_setMoji(int moji);


public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();

public slots:
    void slot_setInfo(QString roomid);
    void slot_addUser(UserShowForm *user );
    void slot_removeUser(UserShowForm *user );
    void slot_refreshUser(int id,QImage &img);
    void slot_removeUser(int id  );
    void slot_clearUser();
    void slot_setAudioCheck( bool check);
    void slot_setVideoCheck( bool check);
    void closeEvent(QCloseEvent *event);

    void slot_setBigImgID(int id ,QString name);
private slots:
    void on_pb_exit_clicked();

    void on_cb_auvio_clicked();

    void on_cb_video_clicked();

    void on_cb_desk_clicked();

    void on_cb_moo_currentIndexChanged(int index);

private:
    Ui::RoomDialog *ui;
    QVBoxLayout*m_mainLayout;
    std:: map<int ,UserShowForm*> map_IdToUserShow;

};

#endif // ROOMDIALOG_H
