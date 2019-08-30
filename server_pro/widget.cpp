#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <cstring>
#include<QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->setupUi(this);
    this->setWindowTitle("服务器 端口12345");

    tcpserver=new QTcpServer(this);
    tcpserver->listen(QHostAddress::Any,12345);

    open_database();

    initTcpsocket();

    new_connect();
}

Widget::~Widget()
{
    delete ui;
}
void Widget::initTcpsocket()
{
    for(int i=0;i<1000;i++)
        tcpsocket[i]=nullptr;
}
void Widget::open_database()
{
    db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("127.0.0.1");

    db.setUserName("root");

    db.setPassword("123456");

    db.setDatabaseName("info");

    db.setPort(3838);

   if(!db.open())
   {
         QMessageBox::warning(this,"错误",db.lastError().text());

         return;
   }

}
void Widget::new_connect()
{
    connect(tcpserver,&QTcpServer::newConnection,
            [=]()
            {
                for(int i=0;i<=index;i++)
                {
                    if(tcpsocket[i]==nullptr)
                    {
                        tcpsocket[i]=(tcpserver->nextPendingConnection());

                        thread_deal(i);

                        if(i==index)
                            index++;

                        QString ip=tcpsocket[i]->peerAddress().toString();
                        qint64 port=tcpsocket[i]->peerPort();

                        QString tmp=QString ("[%1]:[%2]成功连接").arg(ip).arg(port);
                        ui->textread->append(tmp);

                        break;
                    }
                }
            }
             );
}
void Widget::thread_deal(int i)
{
    connect(tcpsocket[i],&QTcpSocket::disconnected,
            [=]()
            {
                tcpsocket[i]->close();
                tcpsocket[i]=nullptr;
                qDebug()<<"clientClose" <<"tcpsocket" <<i;
            });

    connect(tcpsocket[i],&QTcpSocket::readyRead,
            [=]()
            {
            });
}
