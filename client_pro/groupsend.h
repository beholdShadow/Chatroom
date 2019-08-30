#ifndef GROUPSEND_H
#define GROUPSEND_H

#include <QDialog>
#include <QFileDialog>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include <QTcpServer>
#include <QTimer>

namespace Ui {
class groupSend;
}

class groupSend : public QDialog
{
    Q_OBJECT

public:
    explicit groupSend(QWidget *parent = nullptr);
    ~groupSend();

    void senddata();
    void new_connect();
protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_buttonfile_clicked();

    void on_buttonsend_clicked();

    void on_buttonquit_clicked();

private:
    Ui::groupSend *ui;

    QTcpSocket *tcpsocket=nullptr;
    QTcpServer *tcpserver=nullptr;

    QFile  file;
    QString filename;
    qint64 filesize;
    qint64 sendsize;
    QTimer mytimer;
};

#endif // GROUPSEND_H
