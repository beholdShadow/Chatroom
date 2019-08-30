#include "main.h"

QTcpSocket* tcpsocket[1000];
QSqlDatabase db;
QTcpServer* tcpserver=nullptr;
int index=0;
QThreadPool threadpool;
Task t;

void initTcpsocket()
{
    tcpserver=new QTcpServer;
    tcpserver->listen(QHostAddress::Any,12345);

    for(int i=0;i<1000;i++)
        tcpsocket[i]=nullptr;

}

void open_database()
{
    db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("127.0.0.1");

    db.setUserName("root");

    db.setPassword("123456");

    db.setDatabaseName("info");

    db.setPort(3838);

    if(!db.open())
    {
        qDebug()<<"数据库打开失败";
    }
}

void thread_deal(int i)
{
    QObject::connect(tcpsocket[i],&QTcpSocket::disconnected,
            [=]()
            {
                tcpsocket[i]->close();
                tcpsocket[i]=nullptr;
                qDebug()<<"clientClose" <<"tcpsocket" <<i;
            });
    QObject::connect(tcpsocket[i],&QTcpSocket::readyRead,[=]()
             {
                QString array=tcpsocket[i]->readAll();

                QSqlQuery query;

                qDebug()<<"thread_deal threadpool.activeThreadCount()"<<threadpool.activeThreadCount()<<" "<< threadpool.maxThreadCount();;

                QString select=QString (array).section("##",0,0);
                if(select=="注册")
                {
                    QString name=QString (array).section("##",1,1);
                    QString passwd=QString (array).section("##",2,2);

                    QString sql=QString("select id from msg where id='%1'").arg(name);
                    query.exec(sql);
                    if(query.next())
                    {
                        tcpsocket[i]->write("该ID已被注册,请更换其他ID");
                    }
                    else
                    {
                        sql=QString("insert msg values('%1','%2')").arg(name).arg(passwd);
                        query.exec(sql);

                        sql=QString("insert status values('%1',-1,':/Image/qq.png','Welcome to my software')").arg(name).arg(passwd);
                        query.exec(sql);

                        sql=QString("create table %1(id varchar(255),status int,socket int,image varchar(255),filesocket int)").arg(name);
                        query.exec(sql);

                        sql=QString("create table %1(groupchat varchar(255), port int, groupstatus int,manage int))").arg(name+"group");
                        query.exec(sql);

                        tcpsocket[i]->write("注册成功");
                    }
                }
                else if(select=="登录")
                {
                    QString name=QString (array).section("##",1,1);
                    QString passwd=QString (array).section("##",2,2);

                    QString sql=QString("select * from msg where id='%1' and passwd='%2'").arg(name).arg(passwd);
                    query.exec(sql);

                    if(query.next())
                    {
                        QString sql=QString("select socket from status where id='%1'").arg(name);
                        query.exec(sql);

                        if(query.next())
                        {
                            if(query.value(0).toInt()<0)
                            {
                                tcpsocket[i]->write("登陆成功");
                            }
                            else
                            {
                                tcpsocket[i]->write("该账号已经在线,请更换其他账号");
                            }
                        }
                    }
                    else
                        tcpsocket[i]->write("用户名或密码错误，请重新输入");
                }
                else if(select=="功能请求")
                {
                    QString myname=QString (array).section("##",1,1);
                    QString sql=QString("update status set socket=%1 where id='%2'").arg(i).arg(myname);
                    query.exec(sql);

                    sql=QString("select id from %1").arg(myname);
                    query.exec(sql);

                    while(query.next())
                    {
                        QSqlQuery queryII;
                        sql=QString("select socket from status where id='%1'and socket>=0").arg(query.value(0).toString());
                        queryII.exec(sql);
                        if(queryII.next())
                        {
                            int j=queryII.value(0).toInt();
                            QString str=QString("好友上线##%1##").arg(myname);
                            tcpsocket[j]->write(str.toUtf8());

                            str=QString("好友上线##%1##").arg(query.value(0).toString());
                            tcpsocket[i]->write(str.toUtf8());

                        }
                    }
                }
                else if(select=="功能退出")
                {
                    QString myname=QString (array).section("##",1,1);
                    QString newSIGN=QString (array).section("##",2,2);
                    QString sql=QString("update status set socket=-1,sign='%1'where id='%2'").arg(newSIGN).arg(myname);
                    query.exec(sql);
                    sql=QString("update %1 set status=-1").arg(myname);
                    query.exec(sql);

                    sql=QString("select id from %1").arg(myname);
                    query.exec(sql);
                    while(query.next())
                    {
                        QSqlQuery queryII;
                        sql=QString("select socket from status where id='%1'and socket>=0").arg(query.value(0).toString());
                        queryII.exec(sql);

                        if(queryII.next())
                        {
                            int j=queryII.value(0).toInt();
                            QString str=QString("好友下线##%1##").arg(myname);
                            tcpsocket[j]->write(str.toUtf8());
                        }
                    }
                }
                else if(select=="功能")
                {
                    QString choose=QString (array).section("##",1,1);

                    QString image;

                    if(choose=="添加请求")
                    {
                        QString privatename=QString (array).section("##",2,2);
                        QString myname=QString (array).section("##",3,3);
                        QString sql=QString("select image from status where id='%1'").arg(myname);
                        query.exec(sql);
                        if(query.next())
                            image=query.value(0).toString();

                        sql=QString("select socket from status where id='%1'and socket>=0").arg(privatename);
                        query.exec(sql);

                        if(query.next())
                        {
                            int j=query.value(0).toInt();
                            QString str=QString("添加请求##%1##%2").arg(myname).arg(image);
                            tcpsocket[j]->write(str.toUtf8());
                        }
                        else
                        {
                            tcpsocket[i]->write("系统提示##没有搜索到符合ID的用户");
                        }
                    }
                    else if(choose=="拒绝添加请求"|| choose=="接受添加请求")
                    {
                        QString privatename=QString (array).section("##",2,2);
                        QString myname=QString (array).section("##",3,3);
                        QString sql=QString("select image from status where id='%1'").arg(myname);
                        query.exec(sql);
                        if(query.next())
                            image=query.value(0).toString();

                        sql=QString("select socket from status where id='%1'and socket>=0").arg(privatename);
                        query.exec(sql);

                        while(query.next())
                        {
                            int j=query.value(0).toInt();
                            QString str=QString("%1##%2##%3").arg(choose).arg(myname).arg(image);
                            tcpsocket[j]->write(str.toUtf8());
                        }

            //                        if(choose=="接受添加请求")
            //                        {
            //                            sql=QString("insert into %1  values('%2',-1,-1,NULL)").arg(privatename).arg(myname);
            //                            query.exec(sql);
            //                            sql=QString("insert into %1  values('%2',-1,-1,NULL)").arg(myname).arg(privatename);
            //                            query.exec(sql);
            //                        }
                    }
                    else if(choose=="删除请求")
                    {
                        QString privatename=QString (array).section("##",2,2);
                        QString myname=QString (array).section("##",3,3);
                        QString sql=QString("select socket from status where id='%1'and socket>=0").arg(privatename);
                        query.exec(sql);

                        while(query.next())
                        {
                            int j=query.value(0).toInt();
                            QString str=QString("删除请求##%1").arg(myname);
                            tcpsocket[j]->write(str.toUtf8());
                        }
            //                        sql=QString("delete from %1  where id='%2'").arg(privatename).arg(myname);
            //                        query.exec(sql);
            //                        sql=QString("delete from %1  where id='%2'").arg(myname).arg(privatename);
            //                        query.exec(sql);

                    }
                    else if(choose=="进群请求")
                    {
                        QString groupname=QString (array).section("##",2,2);
                        QString Applicate_name=QString (array).section("##",3,3);
                        QString sql=QString("select socket from status where id=(select manage from groupinfo where name='%1')").arg(groupname);
                        query.exec(sql);

                        if(query.next())
                        {
                            QString str=QString("进群请求##%1##%2").arg(groupname).arg(Applicate_name);
                            tcpsocket[query.value(0).toInt()]->write(str.toUtf8());
                        }
                        else
                        {
                            tcpsocket[i]->write("系统提示##没有搜索到符合名称的群");
                        }

                    }
                    else if(choose=="接受进群请求"||choose=="拒绝进群请求")
                    {
                        int port=0;
                        QString groupname=QString (array).section("##",2,2);
                        QString Applicate_name=QString (array).section("##",3,3);

                        QString sql=QString("select port from groupinfo where name='%1'").arg(groupname);
                        query.exec(sql);
                        if(query.next())
                            port=query.value(0).toInt();
                        sql=QString("select socket from status where id='%1'").arg(Applicate_name);
                        query.exec(sql);

                        if(query.next())
                        {
                            int j=query.value(0).toInt();
                            QString str=QString("%1##%2##%3").arg(choose).arg(groupname).arg(port);
                            tcpsocket[j]->write(str.toUtf8());
                        }

                    }
                    else if(choose=="建群请求")
                    {
                        qDebug()<<QString(array);
                        QString groupname=QString (array).section("##",2,2);
                        QString Applicate_name=QString (array).section("##",3,3);

                        QString sql=QString("select port from groupinfo where name='%1'").arg(groupname);
                        query.exec(sql);
                        if(query.next())
                        {
                            tcpsocket[i]->write("系统提示##群聊名称重复，请更换其他名称");
                        }
                        else
                        {
                            sql=QString("insert into groupinfo (name,manage) values('%1','%2')").arg(groupname).arg(Applicate_name);
                            query.exec(sql);

                            sql=QString("select port from groupinfo where name='%1'").arg(groupname);
                            query.exec(sql);

                            if(query.next())
                            {
                                QString str=QString("建群回复##%1##%2").arg(groupname).arg(query.value(0).toInt());
                                tcpsocket[i]->write(str.toUtf8());
                            }
                        }
                    }
            //                    else if(choose=="修改ID")
            //                    {
            //                        QString myname=QString (array).section("##",2,2);
            //                        QString newID=QString (array).section("##",3,3);
            //                        QString sql=QString("update status set id='%1' where id='%2'").arg(newID).arg(myname);
            //                        query.exec(sql);
            //                        tcpsocket[i]->write("系统提示##用户ID修改成功");
            //                        sql=QString("select id from %1").arg(myname);
            //                        query.exec(sql);
            //                        while(query.next())
            //                        {
            //                            QSqlQuery queryII;
            //                            sql=QString("select socket from status where id='%1'and socket>=0").arg(query.value(0).toString());
            //                            queryII.exec(sql);


            //                            if(queryII.next())
            //                            {
            //                                int j=queryII.value(0).toInt();
            //                                QString str=QString("好友ID##%1##%2##").arg(myname).arg(newID);
            //                                tcpsocket[j]->write(str.toUtf8());
            //                            }
            //                        }

            //                    }
                    else if(choose=="修改密码")
                    {
                        QString myname=QString (array).section("##",2,2);
                        QString newPASSWD=QString (array).section("##",3,3);
                        QString sql=QString("update msg set passwd='%1' where id='%2'").arg(newPASSWD).arg(myname);
                        query.exec(sql);
                        tcpsocket[i]->write("系统提示##用户密码修改成功");
                    }
                    else if(choose=="修改头像")
                    {
                        QString myname=QString (array).section("##",2,2);
                        QString image=QString (array).section("##",3,3);

                        QString sql=QString("update status set image='%1' where id='%2'").arg(image).arg(myname);
                        query.exec(sql);

                        sql=QString("select socket from status where id in (select id from %1 where status>0)").arg(myname);
                        query.exec(sql);

                        while(query.next())
                        {
                            int j=query.value(0).toInt();
                            tcpsocket[j]->write(("好友修改头像##"+myname+"##"+image+"##").toUtf8());
                        }
                    }
                }
                else if(select=="私聊请求")
                {
                    QString privatename=QString (array).section("##",1,1);
                    QString myname=QString (array).section("##",2,2);

                    QString sql=QString("update %1 set socket=%2 where id='%3'").arg(myname).arg(i).arg(privatename);
                    query.exec(sql);

                }
                else if(select=="私聊")
                {
                    QString  privatename=QString (array).section("##",1,1);
                    QString myname=QString (array).section("##",2,2);
                    QString message=QString (array).section("##",3,3);

                    QString sql=QString("select socket from %1 where id='%2' and socket>=0").arg(privatename).arg(myname);
                    query.exec(sql);
                    if(query.next())
                    {
                        int j=query.value(0).toInt();

                        if(message=="退出私聊")
                        {
                            tcpsocket[j]->write("对方退出聊天");
                            sql=QString("update %1 set socket=-1 where id='%2'").arg(myname).arg(privatename);
                            query.exec(sql);
                        }
                        else if(message=="进入私聊")
                        {
                            tcpsocket[j]->write("对方进入聊天");
                            tcpsocket[i]->write("对方进入聊天");
                        }
                        else
                        {
                            tcpsocket[j]->write(message.toUtf8());
                        }
                    }
                    else
                    {
                        if(message=="退出私聊")
                        {
                            sql=QString("update %1 set socket=-1 where id='%2'").arg(myname).arg(privatename);
                            query.exec(sql);
                        }
                    }
                }
                else if (select=="文件信息")
                {
                    QString privatename=QString (array).section("##",1,1);
                    QString myname=QString (array).section("##",2,2);
                    QString fileinfo=QString (array).section("##",3,3);
                    QString str=QString("%1##%2").arg(select).arg(fileinfo);

                    qDebug()<<"文件信息fileinfo"<<fileinfo;
                    QString sql=QString("update %1 set filesocket=%2 where id='%3' ").arg(myname).arg(i).arg(privatename);
                    query.exec(sql);
                    sql=QString("select socket from %1 where id='%2' and socket>=0").arg(privatename).arg(myname);
                    query.exec(sql);

                    if(query.next())
                    {
                        int j=query.value(0).toInt();

                        tcpsocket[j]->write(str.toUtf8());
                    }
                }
                else if (select=="文件回复")
                {
                    QString privatename=QString (array).section("##",1,1);
                    QString myname=QString (array).section("##",2,2);
                    QString fileinfo=QString (array).section("##",3,3);

                    qDebug()<<"文件回复fileinfo"<<fileinfo;
                    QString sql=QString("select filesocket from %1 where id='%2'").arg(privatename).arg(myname);
                    query.exec(sql);

                    if(query.next())
                    {
                        int j=query.value(0).toInt();

                        tcpsocket[j]->write(fileinfo.toUtf8());
                    }
                }

             });
}

void new_connect()
{
    QObject::connect(tcpserver,&QTcpServer::newConnection,[=]()
            {
                for(int i=0;i<=index;i++)
                {
                    if(tcpsocket[i]==nullptr)
                    {
                        tcpsocket[i]=(tcpserver->nextPendingConnection());

                        threadpool.start(new Task(i));

                        qDebug()<<"threadpool.activeThreadCount()"<<threadpool.activeThreadCount()<<" "<< threadpool.maxThreadCount();;

                        if(i==index)
                            index++;

                        QString ip=tcpsocket[i]->peerAddress().toString();
                        qint64 port=tcpsocket[i]->peerPort();

                        QString tmp=QString ("[%1]:[%2]成功连接").arg(ip).arg(port);
                        qDebug()<<tmp;

                        break;
                    }
                }
            }
            );
}

Task::Task(int i)
{
    this->i=i;
}

Task::~Task()
{}

void Task::run()
{
    thread_deal(i);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    threadpool.setMaxThreadCount(10);

    open_database();

    initTcpsocket();

    new_connect();


    return a.exec();
}

