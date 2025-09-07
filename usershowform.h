#ifndef USERSHOWFORM_H
#define USERSHOWFORM_H

#include <QWidget>
#include<QPaintEvent>
#include<QImage>
#include<QPainter>
#include<QMouseEvent>
#include<QTimer>
#include<QTime>
namespace Ui {
class UserShowForm;
}

class UserShowForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserShowForm(QWidget *parent = nullptr);
    ~UserShowForm();
signals:
    void sig_itemCliecked(int id,QString name);
public slots:
    void slot_SetInfo(int id ,QString name);
    void paintEvent(QPaintEvent *event);
    void slot_setImage(QImage &img);
    void mousePressEvent(QMouseEvent *event);
    void  slot_checkTimer();
private:
    Ui::UserShowForm *ui;
    int m_id;
    QString m_userName;
    QImage m_img;
    friend class RoomDialog;
    QImage m_defalutImg;
    QTimer m_timer;
    QTime m_lastTime;
};

#endif // USERSHOWFORM_H
