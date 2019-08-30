#ifndef SENDER_H
#define SENDER_H

#include <QDialog>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include <QTcpServer>
#include <QMovie>
#include "filethread.h"

namespace Ui {
class sender;
}

class sender : public QDialog
{
    Q_OBJECT

public:
    explicit sender(QWidget *parent,QString myname,QString privatename);
    ~sender();

    void senddata();
    void new_connect();

    void dealFile(qint64 writtenBytes);
protected:
     void closeEvent(QCloseEvent *);

private slots:
    void on_buttonfile_clicked();

    void on_buttonsend_clicked();

    void on_buttonquit_clicked();   

private:
    Ui::sender *ui;
    QMovie GIF;
    QString myname;
    QString privatename;

    QTcpSocket *tcpsocket=nullptr;
    QTcpServer *tcpserver=nullptr;
    QTcpSocket *Specialsocket=nullptr;

    QFile  file;
    QString filename;
    qint64 filesize;
    qint64 sendsize;

    Filethread *thread=nullptr;
};

#endif // SENDER_H
