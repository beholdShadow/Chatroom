#include "drawer.h"
#include "ui_drawer.h"
#include "widget.h"
#include "group.h"

drawer::drawer(QWidget *parent,QString usrname):
    QWidget(parent),
    ui(new Ui::drawer)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    mytimer=new QTimer(this);
    myname=usrname;
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);    

    open_database();

    paintdrawer();

    tcpsocket=new QTcpSocket(this);
    tcpsocket->connectToHost(QHostAddress("127.0.0.1"),12345);

    connect(tcpsocket,&QTcpSocket::connected,
            [=]()
            {
                QString str=QString("功能请求##%1").arg(myname);
                tcpsocket->write(str.toUtf8());
            });

    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray array=tcpsocket->readAll();
                QSqlQuery query;

                qDebug()<<QString(array);

                QString select=QString (array).section("##",0,0);
                if(select=="好友上线")
                {
                    int j=1;
                    while(1)
                    {
                        QString name=QString (array).section("##",j,j);
                        if(name=="")
                                break;

                        QString sql=QString("update %1 set status=1 where id='%2'").arg(myname).arg(name);
                        query.exec(sql);

                        for(int i=0;i<privateindex;i++)
                        {
                            if(toolBtn[i].text()==(name+"(下线)"))
                            {
                                toolBtn[i].setText(name);
                                toolBtn[i].setEnabled(true);
                                break;
                            }
                        }
                         j+=2;
                     }

                }
                else if(select=="好友下线")
                {
                    QString name=QString (array).section("##",1,1);
                    QString sql=QString("update %1 set status=-1 where id='%2'").arg(myname).arg(name);
                    query.exec(sql);
                    for(int i=0;i<privateindex;i++)
                    {
                        if(toolBtn[i].text()==name)
                        {
                            toolBtn[i].setText(name+"(下线)");
                            toolBtn[i].setEnabled(false);
                            break;
                        }
                    }
                }
                else if(select=="好友修改头像")
                {
                    QString name=QString (array).section("##",1,1);
                    QString image=QString (array).section("##",2,2);

                    QString sql=QString("update %1 set image='%2' where id='%3'").arg(myname).arg(image).arg(name);
                    query.exec(sql);

                    for(int i=0;i<privateindex;i++)
                    {
                        if(toolBtn[i].text()==name)
                        {
                            toolBtn[i].setIcon(QPixmap(image));
                            toolBtn[i].setIconSize(QPixmap(image).size());
                            break;
                        }
                    }
                }
                else if(select=="添加请求")
                {
                    QString name=QString (array).section("##",1,1);

                    QString str=QString("是否接受%1好友请求？").arg(name);
                    int ret=QMessageBox::question(this,tr("添加好友请求"),str);
                    switch (ret)
                    {
                        case QMessageBox::Yes:
                            {
                                QString image=QString (array).section("##",2,2);
                                QString sql=QString("insert into %1 values('%2',1,-1,'%3',-1)").arg(myname).arg(name).arg(image);
                                QString str=QString("功能##接受添加请求##%1##%2").arg(name).arg(myname);
                                query.exec(sql);
                                tcpsocket->write(str.toUtf8());


                                int i=privateindex;
                                privateindex++;

                                toolBtn[i].setText(name);
                                toolBtn[i].setIcon(QPixmap(image));
                                toolBtn[i].setIconSize(QPixmap(image).size());
                                toolBtn[i].setAutoRaise(true);
                                toolBtn[i].setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                                connect(&toolBtn[i],&QToolButton::clicked,
                                        [=]()
                                        {
                                            QSqlQuery query;
                                            QString sql=QString("select socket from %1 where id='%2'and socket>=0").arg(myname).arg(toolBtn[i].text());
                                            query.exec(sql);
                                            if(!query.next())
                                            {
                                                Widget *chat= new Widget(nullptr,myname,toolBtn[i].text(), db);
                                                chat->setWindowTitle(toolBtn[i].text());
                                                chat->setWindowIcon(toolBtn[i].icon());
                                                chat->resize(640,409);
                                                chat->show();
                                           }
                                        });
                                toolBtn[i].show();
                                layout->removeItem(vSpacer);
                                layout->addWidget(&toolBtn[i]);
                                layout->addSpacerItem(vSpacer);

                                break;
                            }
                        case QMessageBox::No:
                            {
                                QString str=QString("功能##拒绝添加请求##%1##%2").arg(name).arg(myname);
                                tcpsocket->write(str.toUtf8());
                                break;
                            }
                    }
                }
                else if(select=="接受添加请求")
                {
                    QString name=QString (array).section("##",1,1);
                    QString image=QString (array).section("##",2,2);
                    QString sql=QString("insert into %1 values('%2',1,-1,'%3',-1)").arg(myname).arg(name).arg(image);
                    qDebug()<<sql;
                    query.exec(sql);

                    int i=privateindex;
                    toolBtn[i].setText(name);
                    toolBtn[i].setIcon(QPixmap(image));
                    toolBtn[i].setIconSize(QPixmap(image).size());
                    toolBtn[i].setAutoRaise(true);
                    toolBtn[i].setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                    connect(&toolBtn[i],&QToolButton::clicked,
                            [=]()
                            {
                                QSqlQuery query;
                                QString sql=QString("select socket from %1 where id='%2'and socket>=0").arg(myname).arg(toolBtn[i].text());
                                query.exec(sql);
                                if(!query.next())
                                {
                                    Widget *chat= new Widget(nullptr,myname,toolBtn[i].text(), db);
                                    chat->setWindowTitle(toolBtn[i].text());
                                    chat->setWindowIcon(toolBtn[i].icon());
                                    chat->resize(640,409);
                                    chat->show();
                                }
                            });
                    privateindex++;

                    layout->removeItem(vSpacer);
                    layout->addWidget(&toolBtn[i]);
                    layout->addSpacerItem(vSpacer);

                }
                else if(select=="拒绝添加请求")
                {
                    QMessageBox::information(this,tr("添加失败"),tr("对方拒绝你的请求"));

                }
                else if(select=="删除请求")
                {
                    QString name=QString (array).section("##",1,1);

                    QString sql=QString("delete from %1 where id='%2'").arg(myname).arg(name);
                    query.exec(sql);

                    for(int i=0;i<privateindex;i++)
                    {
                        qDebug()<<toolBtn[i].text();
                        if(toolBtn[i].text()==name)
                        {
                            layout->removeWidget(&toolBtn[i]);
                            toolBtn[i].setText("");
                            toolBtn[i].hide();

                            break;
                        }
                    }
                }
                else if(select=="进群请求")
                {
                    qDebug()<<QString (array);
                    QString group=QString (array).section("##",1,1);
                    QString name=QString (array).section("##",2,2);
                    QString str=QString("是否接受 %1 进入 %2 的群聊请求？").arg(name).arg(group);
                    int ret=QMessageBox::question(this,tr("进群请求"),str);
                    switch (ret)
                    {
                        case QMessageBox::Yes:
                            {
                                QString str=QString("功能##接受进群请求##%1##%2").arg(group).arg(name);
                                tcpsocket->write(str.toUtf8());

                                break;
                            }
                        case QMessageBox::No:
                            {
                                QString str=QString("功能##拒绝进群请求##%1##%2").arg(group).arg(name);
                                tcpsocket->write(str.toUtf8());

                                break;
                            }
                    }
                }
                else if(select=="接受进群请求"||select=="建群回复")
                {
                    qDebug()<<QString(array);
                    QString groupname=QString(array).section("##",1,1);
                    QString port=QString(array).section("##",2,2);

                    QString sql;
                    if(select=="接受进群请求")
                    {
                        sql=QString("insert into %1 values('%2',%3,-1,-1)").arg(myname+"group").arg(groupname).arg(port.toInt());
                    }
                    else
                    {
                        sql=QString("insert into %1 values('%2',%3,-1,1)").arg(myname+"group").arg(groupname).arg(port.toInt());
                    }
                    query.exec(sql);

                    int i=groupindex;
                    groupBtn[i].setText(groupname);
                    groupBtn[i].setIcon(QPixmap(":/Image/groupchat.jpg"));
                    groupBtn[i].setIconSize(QPixmap(":/Image/groupchat.jpg").size());
                    groupBtn[i].setAutoRaise(true);
                    groupBtn[i].setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                    connect(&groupBtn[i],&QToolButton::clicked,
                            [=]()
                            {
                                QSqlQuery query;
                                QString sql=QString("select  groupchat from %1 where groupchat='%2' and groupstatus=1").arg(myname+"group").arg(groupBtn[i].text());
                                query.exec(sql);
                                if(!query.next())
                                {
                                    group *chat=new group(nullptr,myname, port.toInt(),groupBtn[i].text(),db);
                                    chat->setWindowTitle(groupBtn[i].text());
                                    chat->setWindowIcon(groupBtn[i].icon());
                                    chat->resize(640,409);
                                    chat->show();
                                }
                            });
                    groupindex++;

                    layoutGroup->removeItem(vSpacer);
                    layoutGroup->addWidget(&groupBtn[i]);
                    layoutGroup->addSpacerItem(vSpacer);

                }
                else if(select=="拒绝进群请求")
                {
                    QMessageBox::information(this,tr("进群失败"),tr("管理员拒绝你的进群请求"));

                }
                else if(select=="系统提示")
                {
                    QString message=QString (array).section("##",1,1);
                    QMessageBox::information(this,"系统提示",message);
                }
            });
}

drawer::~drawer()
{
    delete ui;
}
void drawer::closeEvent(QCloseEvent *)
{
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    tcpsocket=nullptr;
}
void drawer::open_database()
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
void drawer::mousePressEvent(QMouseEvent *event)
{
    P =event->globalPos()-frameGeometry().topLeft();
}
void drawer::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPos()-P);
}

void drawer::paintdrawer()
{
    NAME=new QLabel;
    NAME->setText(myname);
    SIGN=new QLineEdit;
    SIGN->setEnabled(false);
    image=new QToolButton;

    QSqlQuery query;
    QString sql=QString("select image ,sign from status where id='%1'").arg(myname);
    query.exec(sql);
    if(query.next())
     {
        image->setIcon(QPixmap(query.value(0).toString()));
        image->setIconSize(QPixmap(query.value(0).toString()).size());
        image->setAutoRaise(true);

        SIGN->setText(query.value(1).toString());

    }
    connect(image,&QToolButton::clicked,
            [=]()
            {
                QString path=QFileDialog::getOpenFileName(this,tr("Select an image"),"E:/File/Mydemo/Project/Chatroom_project/Image",
                                                          tr("PNG (*.png)\n"
                                                              "JPEG (*.jpg *jpeg)\n"
                                                              "Bitmap Files (*.bmp)\n"
                                                             "GIF (*.gif)\n"
                                                             ));
                if(!path.isEmpty())
                {
                    QFileInfo info(path);
                    QString Imagename=info.fileName();

                    image->setIcon(QPixmap(":/Image/"+Imagename));
                    image->setIconSize(QPixmap(":/Image/"+Imagename).size());

                    tcpsocket->write(("功能##修改头像##"+myname+"##:/Image/"+Imagename).toUtf8());
                }

            });
    SIGN->setStyleSheet(QString("font: 10pt Arial;"));

    QPushButton *change=new QPushButton("修改");
    change->setStyleSheet(QString("QPushButton { border: 2px solid #8f8f91;"
                                "border-radius: 6px;"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 #f6f7fa, stop: 1 #dadbde);}"));
    connect(change,&QPushButton::clicked,
            [=]()
            {
                SIGN->setEnabled(true);
             });
    QPushButton *submit=new QPushButton("提交");
    submit->setStyleSheet(QString("QPushButton { border: 2px solid #8f8f91;"
                                "border-radius: 6px;"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 #f6f7fa, stop: 1 #dadbde);}"));
    connect(submit,&QPushButton::clicked,
            [=]()
            {
                SIGN->setEnabled(false);
            });
    QPushButton *QUIT=new QPushButton("退出");
    QUIT->setStyleSheet(QString("QPushButton { border: 2px solid #8f8f91;"
                                "border-radius: 6px;"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 #f6f7fa, stop: 1 #dadbde);}"));
    connect(QUIT,&QPushButton::clicked,
            [=]()
            {                
                QString str=QString("功能退出##%1##%2").arg(myname).arg(SIGN->text());
                tcpsocket->write(str.toUtf8());

                mytimer->start(100);
             });
    connect(mytimer,&QTimer::timeout,
            [=]()
            {
                this->close();
            });

    QHBoxLayout *first=new QHBoxLayout;
    first->addWidget(change);
    first->addWidget(submit);
    QHBoxLayout *second=new QHBoxLayout;
    second->addWidget(NAME);
    second->addWidget(QUIT);
    QVBoxLayout *third=new QVBoxLayout;
    third->addLayout(second);
    third->addLayout(first);
    QHBoxLayout *Hlayout=new QHBoxLayout;
    Hlayout->addWidget(image);
    Hlayout->addLayout(third);

    vSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    privateBox=new QGroupBox;
    layout=new QVBoxLayout(privateBox);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignLeft);

    sql=QString("select id, image ,status from %1").arg(myname);
    query.exec(sql);

    while(query.next())
    {
        int i=privateindex;
        if(query.value(2).toInt()>=0)
        {
            toolBtn[i].setText(query.value(0).toString());
            toolBtn[i].setEnabled(true);
         }
        else
        {
            toolBtn[i].setText(query.value(0).toString()+"(下线)");
            toolBtn[i].setEnabled(false);
        }
        toolBtn[i].setIcon(QPixmap(query.value(1).toString()));
        toolBtn[i].setIconSize(QPixmap(query.value(1).toString()).size());
        toolBtn[i].setAutoRaise(true);
        toolBtn[i].setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        layout->addWidget(&toolBtn[i]);

        connect(&toolBtn[i],&QToolButton::clicked,
                [=]()
                {
                    QSqlQuery query;
                    QString sql=QString("select socket from %1 where id='%2'and socket>=0").arg(myname).arg(toolBtn[i].text());
                    query.exec(sql);
                    if(!query.next())
                    {
                        Widget *chat= new Widget(nullptr,myname,toolBtn[i].text(), db);
                        qDebug()<<toolBtn[i].text();
                        chat->setWindowTitle(toolBtn[i].text());
                        chat->setWindowIcon(toolBtn[i].icon());
                        chat->resize(640,409);
                        chat->show();
                    }
                });
        privateindex++;
    }
    layout->addSpacerItem(vSpacer);

    groupBox=new QGroupBox;
    layoutGroup=new QVBoxLayout(groupBox);
    layoutGroup->setMargin(10);
    layoutGroup->setAlignment(Qt::AlignLeft);

    sql=QString("select groupchat,port from %1 where port>0").arg(myname+"Group");
    query.exec(sql);

    while(query.next())
    {
        int i=groupindex;

        groupBtn[i].setText(query.value(0).toString());
        groupBtn[i].setIcon(QPixmap(":/Image/groupchat.jpg"));
        groupBtn[i].setIconSize(QPixmap(":/Image/groupchat.jpg").size());
        groupBtn[i].setAutoRaise(true);
        groupBtn[i].setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        my_port[i]=query.value(1).toInt();

        layoutGroup->addWidget(&groupBtn[i]);

        connect(&groupBtn[i],&QToolButton::clicked,
                [=]()
                {
                    QSqlQuery query;
                    QString sql=QString("select groupchat from %1 where groupchat='%2' and groupstatus=1").arg(myname+"group").arg(groupBtn[i].text());
                    query.exec(sql);
                    if(!query.next())
                    {
                        group *chat=new group(nullptr,myname, my_port[i],groupBtn[i].text(), db);
                        chat->setWindowTitle(groupBtn[i].text());
                        chat->setWindowIcon(groupBtn[i].icon());
                        chat->resize(640,409);
                        chat->show();
                    }
                });
        groupindex++;
    }
    layoutGroup->addSpacerItem(vSpacer);

    BOXone=new QToolBox(this);
    BOXone->addItem((QWidget*)privateBox,tr("好友列表"));
    BOXone->addItem((QWidget*)groupBox,tr("群聊列表"));

    functionbox=new QGroupBox;
    layfunction=new QVBoxLayout(functionbox);
    layfunction->setMargin(10);
    layfunction->setAlignment(Qt::AlignLeft);
    QLineEdit *LineEdit=new QLineEdit;
    LineEdit->setPlaceholderText(QStringLiteral("对方ID//群聊名称"));
    QToolButton *Addfriend=new QToolButton;
    Addfriend->setIcon(QPixmap(":/Image/add.jpg"));
    Addfriend->setIconSize(QPixmap(":/Image/add.jpg").size());
    Addfriend->setAutoRaise(true);
    Addfriend->setText(tr("添加好友"));
    Addfriend->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(Addfriend,&QToolButton::clicked,
            [=]()
            {
                if(LineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要添加的对方ID"));
                }
                else
                {
                    QSqlQuery query;
                    QString sql=QString("select id from %1  where id='%2'").arg(myname).arg(LineEdit->text());
                    query.exec(sql);
                    if(query.next())
                    {
                        QMessageBox::information(this,tr("无法添加"),tr("对方已经是你的好友"));
                    }
                    else
                    {
                        QString str=QString("功能##添加请求##%1##%2").arg(LineEdit->text()).arg(myname);
                        tcpsocket->write(str.toUtf8());
                    }
                }
                LineEdit->clear();
            });
    QToolButton *Delfriend=new QToolButton;
    Delfriend->setIcon(QPixmap(":/Image/del.jpg"));
    Delfriend->setIconSize(QPixmap(":/Image/del.jpg").size());
    Delfriend->setAutoRaise(true);
    Delfriend->setText(tr("删除好友"));
    Delfriend->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(Delfriend,&QToolButton::clicked,
            [=]()
            {
                if(LineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要删除的对方ID"));
                }
                else
                {
                    QSqlQuery query;
                    QString sql=QString("select id from %1  where id='%2'").arg(myname).arg(LineEdit->text());
                    query.exec(sql);
                    if(!query.next())
                    {
                        QMessageBox::information(this,tr("无法删除"),tr("你没有这个ID的好友"));
                    }
                    else
                    {
                        sql=QString("delete from %1 where id='%2'").arg(myname).arg(LineEdit->text());
                        query.exec(sql);

                        QString str=QString("功能##删除请求##%1##%2").arg(LineEdit->text()).arg(myname);
                        tcpsocket->write(str.toUtf8());

                        for(int i=0;i<privateindex;i++)
                        {
                            qDebug()<<toolBtn[i].text();

                            if(toolBtn[i].text()==LineEdit->text())
                            {
                                qDebug()<<"匹配到删除按钮信息";
                                layout->removeWidget(&toolBtn[i]);
                                toolBtn[i].setText("");
                                toolBtn[i].hide();

                                break;
                            }
                        }
                    }

                }
                LineEdit->clear();
            });
    QToolButton *Addgroup=new QToolButton;
    Addgroup->setIcon(QPixmap(":/Image/groupchat.jpg"));
    Addgroup->setIconSize(QPixmap(":/Image/groupchat.jpg").size());
    Addgroup->setAutoRaise(true);
    Addgroup->setText(tr("添加群聊"));
    Addgroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(Addgroup,&QToolButton::clicked,
            [=]()
            {
                if(LineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要添加的群聊名称"));
                }
                else
                {
                    QString str=QString("功能##进群请求##%1##%2").arg(LineEdit->text()).arg(myname);
                    tcpsocket->write(str.toUtf8());
                }
                LineEdit->clear();
            });
    QToolButton *Delgroup=new QToolButton;
    Delgroup->setIcon(QPixmap(":/Image/groupquit.jpg"));
    Delgroup->setIconSize(QPixmap(":/Image/groupquit.jpg").size());
    Delgroup->setAutoRaise(true);
    Delgroup->setText(tr("删除群聊"));
    Delgroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(Delgroup,&QToolButton::clicked,
            [=]()
            {
                if(LineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要退出的群聊"));
                }
                else
                {
                    QSqlQuery query;
                    QString sql=QString("select groupchat from %1  where groupchat='%2'").arg(myname+"group").arg(LineEdit->text());
                    query.exec(sql);
                    if(!query.next())
                    {
                        QMessageBox::information(this,tr("无法删除"),tr("你没有进入该群聊"));
                    }
                    else
                    {
                        sql=QString("delete from %1 where groupchat='%2'").arg(myname+"group").arg(LineEdit->text());
                        query.exec(sql);
\
                        for(int i=0;i<groupindex;i++)
                        {
                            if(groupBtn[i].text()==LineEdit->text())
                            {
                                layoutGroup->removeWidget(&groupBtn[i]);
                                groupBtn[i].hide();

                                break;
                            }
                        }
                    }
                }
                LineEdit->clear();
            });
    QToolButton *Creategroup=new QToolButton;
    Creategroup->setIcon(QPixmap(":/Image/groupchat.jpg"));
    Creategroup->setIconSize(QPixmap(":/Image/groupchat.jpg").size());
    Creategroup->setAutoRaise(true);
    Creategroup->setText(tr("创建群聊"));
    Creategroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(Creategroup,&QToolButton::clicked,
            [=]()
            {
                if(LineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要创建的群聊"));
                }
                else
                {
                    QString str=QString("功能##建群请求##%1##%2").arg(LineEdit->text()).arg(myname);
                    tcpsocket->write(str.toUtf8());
                }
                LineEdit->clear();
            });

    layfunction->addWidget(LineEdit);
    layfunction->addWidget(Addfriend);
    layfunction->addWidget(Delfriend);
    layfunction->addWidget(Addgroup);
    layfunction->addWidget(Delgroup);
    layfunction->addWidget(Creategroup);
    layfunction->addSpacerItem(vSpacer);

    Messagebox=new QGroupBox;
    laymessage=new QVBoxLayout(Messagebox);
    laymessage->setMargin(10);
    laymessage->setAlignment(Qt::AlignLeft);
    QLineEdit *lineEdit=new QLineEdit;
    lineEdit->setPlaceholderText(QStringLiteral("修改的个人信息"));
    QToolButton *chname=new QToolButton;
    chname->setIcon(QPixmap(":/Image/groupquit.jpg"));
    chname->setIconSize(QPixmap(":/Image/groupquit.jpg").size());
    chname->setAutoRaise(true);
    chname->setText(tr("修改ID"));
    chname->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(chname,&QToolButton::clicked,
            [=]()
            {
                if(lineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要修改的ID"));
                }
                else
                {
                    QString str=QString("功能##修改ID##%1##%2").arg(myname).arg(lineEdit->text());
                    NAME->setText(lineEdit->text());
                    tcpsocket->write(str.toUtf8());

                }
                lineEdit->clear();
            });
    QToolButton *chpasswd=new QToolButton;
    chpasswd->setIcon(QPixmap(":/Image/groupquit.jpg"));
    chpasswd->setIconSize(QPixmap(":/Image/groupquit.jpg").size());
    chpasswd->setAutoRaise(true);
    chpasswd->setText(tr("修改密码"));
    chpasswd->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(chpasswd,&QToolButton::clicked,
            [=]()
            {
                if(lineEdit->text().isEmpty())
                {
                    QMessageBox::information(this,tr("警告"),tr("请输入要修改的密码"));
                }
                else
                {
                    QString str=QString("功能##修改密码##%1##%2").arg(myname).arg(lineEdit->text());
                    tcpsocket->write(str.toUtf8());
                }
                lineEdit->clear();
            });

    laymessage->addWidget(lineEdit);
    laymessage->addWidget(chname);
    laymessage->addWidget(chpasswd);
    laymessage->addSpacerItem(vSpacer);

    BOXtwo=new QToolBox(this);
    BOXtwo->addItem((QWidget*)functionbox,tr("通信菜单"));
    BOXtwo->addItem((QWidget*)Messagebox,tr("个人信息"));

    tabwidget=new QTabWidget(this);
    tabwidget->addTab(BOXone,"联系人");
    tabwidget->addTab(BOXtwo,"功能");

    layoutWidget=new QVBoxLayout(this);
    layoutWidget->addLayout(Hlayout);
    layoutWidget->addWidget(SIGN);
    layoutWidget->addWidget(tabwidget);
}
