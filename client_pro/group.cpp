#include "group.h"
#include "ui_group.h"
#include <QDateTime>

group::group(QWidget *parent,QString usrname,qint16 port,QString groupname,QSqlDatabase mydb) :
    QWidget(parent),
    ui(new Ui::group)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    ui->setupUi(this);

    db=mydb;

    my_name=usrname;
    my_port=quint16(port);
    my_groupname=groupname;

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


    QSqlQuery query;

    QString sql=QString("update  %1 set groupstatus=1 where groupchat='%2'").arg(my_name+"group").arg(my_groupname);
    query.exec(sql);

    sql=QString("select manage  from %1 where groupchat='%2'").arg(my_name+"group").arg(my_groupname);
    query.exec(sql);
    if(query.next())
    {
        if(query.value(0).toInt()==1)
            isBoss=true;
        else
            isBoss=false;
    }

    if(isBoss)
    {
        ui->buttonKick->setEnabled(true);
        ui->buttonForbid->setEnabled(true);
        ui->buttonUnforbid->setEnabled(true);
        setWindowTitle(tr("管理员"));
    }
    else
    {
        ui->buttonKick->hide();
        ui->buttonForbid->hide();
        ui->buttonUnforbid->hide();
        setWindowTitle(tr("普通成员"));
    }

    udpsocket=new QUdpSocket(this);
    udpsocket->bind(my_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    QString str=QString("进入##%1").arg(my_name);
    QByteArray datagram=str.toUtf8();
    udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

    connect(udpsocket,&QUdpSocket::readyRead,
            [=]()
            {
            QByteArray datagram;
            datagram.resize(int(udpsocket->pendingDatagramSize()));
            udpsocket->readDatagram(datagram.data(), datagram.size());

            QString type=QString (datagram).section("##",0,0);
            if(type=="进入")
            {
                QString name=QString (datagram).section("##",1,1);
                bool isEmpty = ui->tableWidget->findItems(name, Qt::MatchExactly).isEmpty();
                if (isEmpty)
                {
                    ui->tableWidget->insertRow(0);

                    QTableWidgetItem *Myname;

                    Myname = new QTableWidgetItem(name);
                    ui->tableWidget->setItem(0, 0, Myname);

                    QTableWidgetItem *Mystatus = new QTableWidgetItem("127.0.0.1");
                    ui->tableWidget->setItem(0, 1, Mystatus);

                    ui->textBrowser->setTextColor(Qt::gray);
                    ui->textBrowser->setCurrentFont(QFont("Times New Roman", 12));
                    ui->textBrowser->append(tr("%1 在 %2 进入！").arg(name).arg(time));

                    QString str=QString("进入##%1").arg(my_name);
                    QByteArray datagram=str.toUtf8();
                    udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

                }
            }
            else if(type=="离开")
            {
                QString name=QString (datagram).section("##",1,1);
                bool isEmpty = ui->tableWidget->findItems(name, Qt::MatchExactly).isEmpty();
                if (!isEmpty)
                {
                    int row= ui->tableWidget->findItems(name, Qt::MatchExactly).first()->row();
                    ui->tableWidget->removeRow(row);
                    ui->textBrowser->setTextColor(Qt::gray);
                    ui->textBrowser->setCurrentFont(QFont("Times New Roman", 12));
                    ui->textBrowser->append(tr("%1 在 %2 离开！").arg(name).arg(time));
                }
            }
            else if(type=="禁言")
            {
                 QString name=QString (datagram).section("##",1,1);

                 if(my_name==name)
                 {
                    QMessageBox::warning(this, tr("群聊提示"),tr("你被管理员禁言了!!!"));
                    ui->textEditWrite->setEnabled(false);
                    ui->buttonSend->setEnabled(false);
                 }

                 int row= ui->tableWidget->findItems(name, Qt::MatchExactly).first()->row();
                 ui->tableWidget->item(row,0)->setText(name+"(被禁言)");

                 ui->textBrowser->setTextColor(Qt::darkBlue);
                 ui->textBrowser->setCurrentFont(QFont("Times New Roman", 12));
                 ui->textBrowser->append(tr("%1 在 %2 被管理员禁言！").arg(name).arg(time));
            }
            else if(type=="解禁")
            {
                 QString name=QString (datagram).section("##",1,1);

                 if(my_name+"(被禁言)"==name)
                 {
                    QMessageBox::warning(this, tr("群聊提示"),tr("你被管理员解除禁言,注意自己的发言!!!"));
                    ui->textEditWrite->setEnabled(true);
                    ui->buttonSend->setEnabled(true);
                 }

                 int row= ui->tableWidget->findItems(name, Qt::MatchExactly).first()->row();
                 name.chop(5);
                 ui->tableWidget->item(row,0)->setText(name);

                 ui->textBrowser->setTextColor(Qt::darkBlue);
                 ui->textBrowser->setCurrentFont(QFont("Times New Roman", 12));
                 ui->textBrowser->append(tr("%1 在 %2 被管理员解除禁言！").arg(name).arg(time));
            }
            else if(type=="踢人")
            {
                 QString name=QString (datagram).section("##",1,1);

                 if(my_name==name||my_name+"(被禁言)"==name)
                 {
                    this->close();
                 }

                 ui->textBrowser->setTextColor(Qt::gray);
                 ui->textBrowser->append(tr("%1 在 %2 被管理员踢出群聊！").arg(name).arg(time));
            }
            else if(type=="文件")
            {
                 QString name=QString (datagram).section("##",1,1);
                 QString Sender=QString(datagram).section("##",2,2);

                 if(my_name==name||my_name+"(被禁言)"==name)
                 {
                    int ret=QMessageBox::question(this,tr("传送文件"),tr("是否接收来自 %1 的文件").arg(Sender));
                    switch (ret)
                    {
                        case QMessageBox::Yes:{
                                                QString str=QString("文件回复##%1##接受文件").arg(Sender);
                                                QByteArray datagram=str.toUtf8();
                                                udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

                                                receiver * Receiver=new receiver(nullptr,7777);
                                                Receiver->setWindowTitle("接受文件 from "+Sender);
                                                Receiver->setWindowIcon(this->windowIcon());
                                                Receiver->show();

                                                break;
                                              }
                        case QMessageBox::No:{
                                                QString str=QString("文件回复##%1##拒绝文件").arg(Sender);

                                                QByteArray datagram=str.toUtf8();
                                                udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

                                                break;
                                             }
                    }
                 }
            }
            else if(type=="文件回复")
            {
                QString name=QString (datagram).section("##",1,1);
                QString message=QString(datagram).section("##",2,2);

                if(my_name==name||my_name+"(被禁言)"==name)
                {
                    if(message=="接受文件")
                    {
                        groupSend *send=new groupSend(nullptr);
                        send->setWindowTitle("传送文件");
                        send->setWindowIcon(this->windowIcon());
                        send->show();
                    }
                    else
                    {
                        QMessageBox::information(this,tr("提示"),tr("对方拒绝接受文件"));
                    }
                }

            }
            else if(type=="图片")
            {
                QString name=QString(datagram).section("##",1,1);
                Imagename=QString(datagram).section("##",2,2);

                QString path = QString("<img src=\":/Image/%1\"/>").arg(Imagename);

                ui->textBrowser->setTextColor(Qt::darkMagenta);
                ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
                time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                ui->textBrowser->append(time+" "+name+":");
                ui->textBrowser->append("");
                ui->textBrowser->insertHtml(path);

                Imagename=nullptr;
            }
            else
            {
                QString name=QString(datagram).section("##",0,0);
                QString message=QString(datagram).section("##",1,1);
                ui->textBrowser->setTextColor(Qt::darkMagenta);
                ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
                time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                ui->textBrowser->append(time+" "+name+":");
                ui->textBrowser->append("");
                ui->textBrowser->insertHtml(QString(message));

            }

            });

}

group::~group()
{
    delete ui;
}

void group::on_buttonSend_clicked()
{
    QString str;

    if(ui->textEditWrite->toPlainText()!="")
    {
        if(Imagename!=nullptr)
        {
            if(isBoss)
                str=QString("图片##%1##%2").arg(my_name+"(管理员)").arg(Imagename);
            else
                str=QString("图片##%1##%2").arg(my_name).arg(Imagename);

            Imagename=nullptr;
        }
        else
        {
            QString message=ui->textEditWrite->toHtml();

            if(isBoss)
                str=QString("%1##%2").arg(my_name+"(管理员)").arg(message);
            else
                str=QString("%1##%2").arg(my_name).arg(message);
        }

        QByteArray datagram=str.toUtf8();
        udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

        ui->textEditWrite->clear();
    }

}

void group::on_buttonDel_clicked()
{
    ui->textEditWrite->clear();
}

void group::on_buttonQuit_clicked()
{
    this->close();
}

void group::on_buttonForbid_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::information(this,"系统提示","请选择要禁言的成员");
    }
    else
    {
        int row=ui->tableWidget->currentRow();

        QString name=ui->tableWidget->item(row,0)->text();
        QString str=QString("禁言##%1").arg(name);
        QByteArray datagram=str.toUtf8();

        udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);
    }
}

void group::on_buttonKick_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::information(this,"系统提示","请选择要踢出群聊的成员");
    }
    else
    {
        int row=ui->tableWidget->currentRow();

        QString name=ui->tableWidget->item(row,0)->text();

        QString str=QString("踢人##%1").arg(name);
        QByteArray datagram=str.toUtf8();

        udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);
    }
}

void group::on_buttonUnforbid_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::information(this,"系统提示","请选择要解除禁言的成员");
    }
    else
    {
        int row=ui->tableWidget->currentRow();

        QString name=ui->tableWidget->item(row,0)->text();

        QString str=QString("解禁##%1").arg(name);
        QByteArray datagram=str.toUtf8();

        udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);
    }
}

void group::on_buttonImage_clicked()
{
    QString Imagepath= QFileDialog::getOpenFileName(this, tr("Select an image"),
                                      "../../Image", tr("Bitmap Files (*.bmp)\n"
                                        "JPEG (*.jpg *jpeg)\n"
                                        "GIF (*.gif)\n"
                                        "PNG (*.png)\n"));
    QFileInfo info(Imagepath);
    Imagename=info.fileName();

    QUrl Uri ( QString ( "file://%1" ).arg ( Imagepath ) );
    QImage image = QImageReader ( Imagepath ).read();

    QTextDocument * textDocument = ui->textEditWrite->document();
    textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
    QTextCursor cursor = ui->textEditWrite->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth( image.width() );
    imageFormat.setHeight( image.height() );
    imageFormat.setName( Uri.toString() );
    cursor.insertImage(imageFormat);
}

void group::on_buttonBold_clicked(bool checked)
{
    if(checked)
        ui->textEditWrite->setFontWeight(QFont::Bold);
    else
        ui->textEditWrite->setFontWeight(QFont::Normal);
    ui->textEditWrite->setFocus();
}

void group::on_buttonUnderline_clicked(bool checked)
{
    ui->textEditWrite->setFontUnderline(checked);
    ui->textEditWrite->setFocus();
}

void group::on_buttonItalic_clicked(bool checked)
{
    ui->textEditWrite->setFontItalic(checked);
    ui->textEditWrite->setFocus();
}

void group::on_buttonColor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid()){
        ui->textEditWrite->setTextColor(color);
        ui->textEditWrite->setFocus();
    }
}

void group::on_buttonFile_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::information(this,"系统提示","请选择要传送文件的成员");
    }
    else
    {
        int row=ui->tableWidget->currentRow();

        QString name=ui->tableWidget->item(row,0)->text();

        QString str=QString("文件##%1##%2").arg(name).arg(my_name);
        QByteArray datagram=str.toUtf8();

        udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);
    }
}

void group::on_buttonHistory_clicked()
{
    if (ui->textBrowser->document()->isEmpty())
    {
        QMessageBox::warning(nullptr, tr("警告"), tr("聊天记录为空，无法保存！"), QMessageBox::Ok);
    }
    else
    {
        QString path= QFileDialog::getSaveFileName(this,tr("保存聊天记录"), tr("聊天记录"), tr("文本(*.txt);;所有文件(*.*)"));
        if(!path.isEmpty())
        {
            QFile file(path);
            if (!file.open(QFile::WriteOnly | QFile::Text))
            {
                QMessageBox::warning(this, tr("保存文件"),tr("无法保存文件 %1:\n %2").arg(path).arg(file.errorString()));
                return;
            }
            QTextStream out(&file);
            out << ui->textBrowser->toPlainText();
            file.close();
        }
    }
}

void group::on_buttonClear_clicked()
{
    ui->textBrowser->clear();
}

void group::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->textEditWrite->setCurrentFont(f);
    ui->textEditWrite->setFocus();
}

void group::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->textEditWrite->setFontPointSize(arg1.toDouble());
    ui->textEditWrite->setFocus();
}

void group::on_textEditWrite_currentCharFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont(format.font());

    if (format.fontPointSize() < 8)
    {
        ui->comboBox->setCurrentIndex(4);
    }
    else
    {
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(QString::number(format.fontPointSize())));
    }
    ui->buttonBold->setChecked(format.font().bold());
    ui->buttonItalic->setChecked(format.font().italic());
    ui->buttonUnderline->setChecked(format.font().underline());
    color = format.foreground().color();

}

void group::closeEvent(QCloseEvent *)
{
    QSqlQuery query;
    QString sql=QString("update %1 set groupstatus=-1 where groupchat='%2'").arg(my_name+"group").arg(my_groupname);
    query.exec(sql);

    QString str;
    if(ui->textEditWrite->isEnabled()==false)
        str=QString("离开##%1").arg(my_name+"(被禁言)");
    else
        str=QString("离开##%1").arg(my_name);

    QByteArray datagram=str.toUtf8();
    udpsocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast, my_port);

    udpsocket->close();
}


