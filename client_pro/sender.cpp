#include "sender.h"
#include "ui_sender.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>

sender::sender(QWidget *parent,QString myname,QString privatename) :
    QDialog(parent),
    ui(new Ui::sender)
{
    ui->setupUi(this);

    this->myname=myname;
    this->privatename=privatename;

    ui->buttonsend->setEnabled(false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->lineEdit->setEnabled(false);

    GIF.setFileName(":/Image/timg.gif");
    ui->labelMovie->setMovie(&GIF);
    GIF.start();

    new_connect();

}
void sender::new_connect()
{
    tcpserver=new QTcpServer(this);
    tcpserver->listen(QHostAddress::Any,44444);
    Specialsocket=new QTcpSocket(this);
    Specialsocket->connectToHost(QHostAddress("127.0.0.1"),12345);

    connect(tcpserver,&QTcpServer::newConnection,
            [=]()
            {
                tcpsocket=(tcpserver->nextPendingConnection());
            }
             );
    connect(tcpsocket,&QTcpSocket::disconnected,
            [=]()
            {
                tcpsocket->close();
                tcpsocket=nullptr;
            });
    connect(Specialsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray buf=Specialsocket->readAll();

                if(QString(buf)=="接受文件")
                {
                    senddata();
                }
                else
                {
                    QMessageBox::information(this,tr("提示"),tr("对方拒绝接受文件"));
                    ui->buttonfile->setEnabled(true);
                }
    });
}

void sender::dealFile(qint64 writtenBytes)
{
    ui->progressBar->setValue(writtenBytes/1024);

    if(writtenBytes==filesize)
    {
        thread->wait();
        thread->deleteLater();

        file.close();
        ui->buttonfile->setEnabled(true);
        ui->buttonsend->setEnabled(false);
    }

}
sender::~sender()
{
    delete ui;
}

void sender::on_buttonfile_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"open","../");

    if(file.isOpen()==true)
        file.close();

    if(false==path.isEmpty())
    {
        filename.clear();
        filesize=0;
        sendsize=0;

        QFileInfo info(path);
        filename=info.fileName();
        filesize=info.size();

        file.setFileName(path);

        bool ret=file.open(QIODevice::ReadOnly);
        if(ret==true)
        {
           ui->lineEdit->setText(path);
           ui->buttonfile->setEnabled(false);
           ui->buttonsend->setEnabled(true);

           ui->progressBar->setValue(0);
        }
        else
             qDebug()<<"文件打开失败";
    }
    else
    {
             qDebug()<<"文件路径错误";
    }
}

void sender::on_buttonsend_clicked()
{
    QString head=QString("文件信息##%1##%2##%3$$%4").arg(privatename).arg(myname).arg(filename).arg(filesize);

    qint64 len=Specialsocket->write(head.toUtf8());

    if(len <=0)
    {
        qDebug()<<"头部信息发送失败";
        file.close();
        ui->buttonfile->setEnabled(true);

    }
}

void sender::senddata()
{
    ui->progressBar->setMaximum(filesize/1024);
    ui->buttonsend->setEnabled(false);

    thread=new Filethread(nullptr,tcpsocket,&file,filesize);
    thread->start();

    connect(thread,&Filethread::thread_writtenBytes,this,&sender::dealFile,Qt::QueuedConnection);

}

void sender::on_buttonquit_clicked()
{
    this->close();
}

void sender::closeEvent(QCloseEvent *)
{
    Specialsocket->disconnectFromHost();
    Specialsocket->close();
    tcpserver->close();
    tcpserver=nullptr;
    Specialsocket=nullptr;

}
