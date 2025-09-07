#ifndef SCREENREAD_H
#define SCREENREAD_H
#include <QObject>
#include<QImage>
#include<QTimer>
#include<QApplication>
#include<QDesktopWidget>
#include<QBuffer>
#include<QScreen>
#include"common.h"

class ScreenRead : public QObject
{
    Q_OBJECT
public:
    explicit ScreenRead(QObject *parent = nullptr);
    ~ScreenRead();

signals:
    sig_SrceenRead(QImage img);
public slots:
    void slot_SrceenRead();
    void openVideo();
    void closeVideo();
private:
    QTimer *m_timer;

};

#endif // SCREENREAD_H
