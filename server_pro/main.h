#include <QApplication>
#include <QRunnable>
#include <QTcpServer>
#include <QSqlError>
#include <QSqlQuery>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThreadPool>
#include <QObject>
#include <QtConcurrent>
#include <QEventLoop>

extern QTcpSocket* tcpsocket[1000];
extern QSqlDatabase db;
extern QThreadPool threadpool;
extern QTcpServer *tcpserver;
extern int index;

class Task:public QRunnable
{
public:
    Task(int i=0);
    ~Task();

protected:
    void run();
private:
    int i;
};


