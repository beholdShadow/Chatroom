#include "filethread.h"

Filethread::Filethread(QThread *thread,QTcpSocket *tcpsocket,QFile *file,qint64 filesize)
{

    this->socket=tcpsocket;
    this->file=file;
    this->filesize=filesize;
    this->sendsize=0;

}

Filethread::~Filethread()
{

}

void Filethread::run()
{
    char buf[1024*1024]={0};

    qint64 len=0;
    do
    {
        memset(buf,0,sizeof (buf));

        len=file->read(buf,sizeof(buf));

        len=socket->write(buf,len);

        sendsize+=len;

        qDebug()<<"len:"<<len<<" sendsize"<<sendsize;

        socket->waitForBytesWritten(1000);//********等待本次数据传输到接受端后再进入下次循环写入，否则传输大文件会造成程序崩溃

        emit thread_writtenBytes(sendsize);

    }while (len>0);
}
