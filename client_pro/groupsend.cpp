#include "groupsend.h"
#include "ui_groupsend.h"


groupSend::groupSend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::groupSend)
{
    ui->setupUi(this);    

    ui->buttonsend->setEnabled(false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->lineEdit->setEnabled(false);

    new_connect();

    connect(&mytimer,&QTimer::timeout,
            [=]()
            {
                mytimer.stop();
                senddata();
            });

}

groupSend::~groupSend()
{
    delete ui;
}

void groupSend::senddata()
{
    ui->progressBar->setMaximum(int(filesize));
    ui->buttonsend->setEnabled(false);

    qint64 len=0;
    do
    {
        char buf[4*1024]={0};
        len=0;

        len=file.read(buf,sizeof(buf));

        len=tcpsocket->write(buf,len);

        sendsize+=len;

        ui->progressBar->setValue(int(sendsize));

    }while (len>0);

    if(sendsize==filesize)
    {
       file.close();
       ui->buttonfile->setEnabled(true);
       ui->buttonsend->setEnabled(false);
    }
}

void groupSend::new_connect()
{
    tcpserver=new QTcpServer(this);
    tcpserver->listen(QHostAddress::Any,7777);

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
}

void groupSend::on_buttonfile_clicked()
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

void groupSend::on_buttonsend_clicked()
{
    QString head=QString("%3##%4").arg(filename).arg(filesize);

    qint64 len=tcpsocket->write(head.toUtf8());

    if(len>0)
    {
        mytimer.start(20);
    }
    else
    {
        qDebug()<<"头部信息发送失败";
        file.close();
        ui->buttonfile->setEnabled(true);

    }
}

void groupSend::on_buttonquit_clicked()
{
    this->close();
}

void groupSend::closeEvent(QCloseEvent *)
{
    tcpserver->close();
    tcpserver=nullptr;
}
