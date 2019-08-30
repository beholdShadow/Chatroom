#include "receiver.h"
#include "ui_receiver.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>
#define cout qDebug()<<"["<<__FILE__<<"]"<<"["<<__LINE__<<"]"

receiver::receiver(QWidget *parent,QString filename,qint64 filesize,quint16 port) :
    QDialog(parent),
    ui(new Ui::receiver)
{
    ui->setupUi(this);

    this->filesize=filesize;
    this->filename=filename;
    ui->lineName->setEnabled(false);
    ui->lineSize->setEnabled(false);

    recvsize=0;

    UIinit();

    file.setFileName(filename);

    bool isok=file.open(QIODevice::WriteOnly);
    if(!isok)
    {
        qDebug()<<"WRITE ONLY ERROR";
    }

    tcpsocket=new QTcpSocket(this);

    tcpsocket->connectToHost(QHostAddress("127.0.0.1"),port);

    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray buf=tcpsocket->readAll();

                qint64 len=file.write(buf);

                recvsize+=len;

                qDebug()<<"len="<<len<<"recvsize="<<recvsize<<"filesize="<<filesize;

                ui->progressBar->setValue(recvsize/1024);

                if(recvsize==filesize)
                {
                        file.close();
                        this->close();
                }
            }
            );
}
receiver::receiver(QWidget *parent,quint16 port) :
    QDialog(parent),
    ui(new Ui::receiver)
{
    ui->setupUi(this);

    ui->lineName->setEnabled(false);
    ui->lineSize->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);

    tcpsocket=new QTcpSocket(this);

    tcpsocket->connectToHost(QHostAddress("127.0.0.1"),port);

    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray buf=tcpsocket->readAll();
                if(true==start)
                {
                    start=false;
                    filename= QString (buf).section("##",0,0);
                    filesize= QString(buf).section("##",1,1).toInt();
                    recvsize=0;

                    file.setFileName(filename);

                    UIinit();

                    ui->progressBar->setMaximum(filesize/1024);

                    bool isok=file.open(QIODevice::WriteOnly);
                    if(!isok)
                    {

                        qDebug()<<"WRITE ONLY ERROR";
                    }
                }
                else
                {
                    qint64 len=file.write(buf);

                    recvsize+=len;
                    ui->progressBar->setValue(int(recvsize)/1024);

                    if(recvsize==filesize)
                    {
                            file.close();
                            start=true;
                    }
                }
            }
            );
}
void receiver::UIinit()
{   
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(filesize/1024);
    ui->lineName->setText(filename);
    ui->lineSize->setText(QString::number(filesize)+"字节");
}
receiver::~receiver()
{
    delete ui;
}
void receiver::closeEvent(QCloseEvent *)
{
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    tcpsocket=nullptr;
}
