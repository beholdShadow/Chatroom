#include "dialog.h"
#include "ui_dialog.h"
#include "drawer.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->lineName->setPlaceholderText(QStringLiteral("用户名"));
    ui->linePasswd->setPlaceholderText(QStringLiteral("密码"));
    ui->linePasswd->setMaxLength(6);
    ui->linePasswd->setEchoMode(QLineEdit::Password);

    GIFLOGIN.setFileName(":/Image/mario.gif");
    ui->labelGIFLOGIN->setMovie(&GIFLOGIN);
    GIFLOGIN.start();

    login=new Login(nullptr);

    tcpsocket=new QTcpSocket(this);

    tcpsocket->connectToHost(QHostAddress("127.0.0.1"),12345);

    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray array=tcpsocket->readAll();

                if(QString(array)=="登陆成功")
                {
                    drawer *draw=new drawer(nullptr,ui->lineName->text());
                    draw->show();
                    draw->resize(270,800);

                    tcpsocket->disconnectFromHost();
                    tcpsocket->close();
                    QDialog::accept();
                }
                else
                {
                    QMessageBox::information(this,tr("提示"), QString(array));
                }

            });

    connect(login,&Login::mysignal,this,&Dialog::revsignal);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->globalPos()-p);
}
void Dialog::mousePressEvent(QMouseEvent *e)
{
   p=e->globalPos()-frameGeometry().topLeft();
}

void Dialog::on_buttonQuit_clicked()
{
    this->close();
}

void Dialog::on_buttonEnter_clicked()
{
    QString name=ui->lineName->text();
    QString passwd=ui->linePasswd->text();

    QString str=QString("登录##%1##%2").arg(name).arg(passwd);

    tcpsocket->write(str.toUtf8());
}

void Dialog::on_buttonLogin_clicked()
{   
    login->show();
}

void Dialog::revsignal(QString name)
{
    drawer *draw=new drawer(nullptr,name);
    draw->show();
    draw->resize(270,800);

    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    QDialog::accept();
}
