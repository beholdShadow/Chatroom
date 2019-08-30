#include "login.h"
#include "ui_login.h"
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->lineNAME->setPlaceholderText(QStringLiteral("注册ID"));
    ui->linePASSWD->setPlaceholderText(QStringLiteral("注册密码"));
    ui->lineREPEAT->setPlaceholderText(QStringLiteral("重复注册密码"));
    ui->linePASSWD->setMaxLength(6);
    ui->lineREPEAT->setMaxLength(6);
    ui->linePASSWD->setEchoMode(QLineEdit::Password);
    ui->lineREPEAT->setEchoMode(QLineEdit::Password);

    mytcpsocket=new QTcpSocket(this);

    mytcpsocket->connectToHost(QHostAddress("127.0.0.1"),12345);

    connect(mytcpsocket,&QTcpSocket::readyRead,
           [=]()
            {
                QByteArray array=mytcpsocket->readAll();
                if(QString(array)=="注册成功")
                {
                    emit mysignal(ui->lineNAME->text());

                    this->close();
                }
                else
                {
                    QMessageBox::information(this,tr("注册失败"), QString(array));
                }

            });
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{
    if(ui->linePASSWD->text()!=ui->lineREPEAT->text())
    {
        QMessageBox::warning(this,tr("密码警告"),tr("两次密码输入不一致,请重新输入"));
    }
    else
    {
        QString str=QString("注册##%1##%2").arg(ui->lineNAME->text()).arg(ui->linePASSWD->text());
        mytcpsocket->write(str.toUtf8());
    }
    ui->linePASSWD->clear();
    ui->lineREPEAT->clear();
}
void Login::on_pushButton_2_clicked()
{
    this->hide();
}
void Login::mousePressEvent(QMouseEvent *e)
{
    p=e->globalPos()-frameGeometry().topLeft();
}
void Login::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->globalPos()-p);
}

