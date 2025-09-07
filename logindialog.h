#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include"QCloseEvent"
#include <QDialog>

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT
signals:
    void sig_loginCommit(QString tel ,QString password);
    void sig_RegCommit(QString name, QString tel ,QString password);
    void sig_close();
public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();
    void closeEvent(QCloseEvent*event);

private slots:
    void on_pb_log_clicked();

    void on_pbreg_clicked();

private:
    Ui::loginDialog *ui;
};

#endif // LOGINDIALOG_H
