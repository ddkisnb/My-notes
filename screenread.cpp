#include "screenread.h"

ScreenRead::ScreenRead(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT( slot_SrceenRead()) );

}

ScreenRead::~ScreenRead()
{
    if(m_timer){
        m_timer->stop();
        delete m_timer ; m_timer=nullptr;
    }

}

void ScreenRead::slot_SrceenRead()
{
    //获取桌面对象
    QScreen *src = QApplication::primaryScreen();
    QPixmap map = src->grabWindow( QApplication::desktop()->winId()/* ,0,0 , deskRect.width() ,
    deskRect.height()*/);
     QImage image = map.toImage();
    // image = image.scaled( 1600, 900, Qt::KeepAspectRatio, Qt::SmoothTransformation );
     Q_EMIT sig_SrceenRead(image);

}

void ScreenRead::openVideo()
{
    m_timer->start(1000/FRAME_RATE-10);
}

void ScreenRead::closeVideo()
{

    m_timer->stop();
}
