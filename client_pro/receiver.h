#ifndef RECEIVER_H
#define RECEIVER_H

#include <QTcpSocket>
#include <QDialog>
#include <QFile>

namespace Ui {
class receiver;
}

class receiver : public QDialog
{
    Q_OBJECT

public:
    explicit receiver(QWidget *parent,QString filename,qint64 filesize,quint16 port);
    explicit receiver(QWidget *parent,quint16 port);
    ~receiver();
    void UIinit();

protected:
     void closeEvent(QCloseEvent *);
private:
    Ui::receiver *ui;

    QFile  file;

    QString filename;

    qint64 filesize;

    qint64 recvsize;

    QTcpSocket *tcpsocket;

    bool start=true;

};

#endif // RECEIVER_H
