#include "usershowform.h"
#include "ui_usershowform.h"

UserShowForm::UserShowForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserShowForm),m_defalutImg(":/tx/36.jpg")
{
    ui->setupUi(this);
    connect( &m_timer ,SIGNAL(timeout()),this , SLOT(slot_checkTimer()));
    m_lastTime =  QTime::currentTime();
    m_timer.start(1000);

}

UserShowForm::~UserShowForm()
{
    delete ui;
}

void UserShowForm::slot_SetInfo(int id, QString name)
{
    m_id=id;
    m_userName=name;
    QString test=QString("用户名:%1").arg(m_userName);
    ui->lb_name->setText(test);
}

void UserShowForm::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());
    //画视频帧
    if(m_img.size().height()<=0)return;
    m_img=m_img.scaled(this->width(),
                       this->height()-ui->lb_name->height(),
                       Qt::KeepAspectRatio);
    QPixmap pix =QPixmap::fromImage(m_img);
    int x=this->width()-pix.width();//当前宽度-图片高度
    int y=this->height()-pix.height()-ui->lb_name->height();
    x=x/2;

    y=ui->lb_name->height()+y/2;
    painter.drawPixmap(QPoint(x,y),pix);
    painter.end();


}

void UserShowForm::slot_setImage(QImage &img)
{
    m_img=img;
    m_lastTime = QTime::currentTime();
    this->update();
}

void UserShowForm::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    Q_EMIT sig_itemCliecked(m_id,m_userName);
}

void UserShowForm::slot_checkTimer()
{
    //定时检测接收界面是否超时
    if(m_lastTime.secsTo( QTime ::currentTime())>3){
        //设为默认画面
        slot_setImage(m_defalutImg);

    }
}
