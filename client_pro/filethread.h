#ifndef FILETHREAD_H
#define FILETHREAD_H
#include <QWidget>
#include <QThread>
#include <QTcpSocket>
#include <QFile>
class Filethread:public QThread
{
    Q_OBJECT
public:
    Filethread(QThread *parent=nullptr,QTcpSocket *socket=nullptr,QFile *file=nullptr,qint64 filesize=0);
    ~Filethread();

signals:
    void thread_writtenBytes(qint64 writtenBytes);

protected:
    void run();

private:
    QTcpSocket *socket;
    QFile *file;
    qint64 filesize;
    qint64 sendsize;

};

#endif // FILETHREAD_H
