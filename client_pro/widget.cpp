#include "widget.h"
#include "ui_widget.h"
#include<QDateTime>

Widget::Widget(QWidget *parent,QString usrname,QString privatename,QSqlDatabase mydb):
    QWidget(parent),
    ui(new Ui::Widget)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    my_name=usrname;
    private_name=privatename;

    db=mydb;

    ui->buttonSend->setEnabled(false);
    ui->buttonFile->setEnabled(false);

//    QString sql=QString("update %1 set socket=1 where id='%2'").arg(my_name).arg(private_name);
//    query.exec(sql);

    tcpsocket=new QTcpSocket(this);
    tcpsocket->connectToHost(QHostAddress("127.0.0.1"),12345);

    ui->tableWidget->setRowCount(1);
    QTableWidgetItem *Myname = new QTableWidgetItem(my_name+"(自己)");
    ui->tableWidget->setItem(0, 0, Myname);
    QTableWidgetItem *Mystatus = new QTableWidgetItem("127.0.0.1");
    ui->tableWidget->setItem(0, 1, Mystatus);

    connect(tcpsocket,&QTcpSocket::connected,
            [=]()
            {
               QString str=QString("私聊请求##%1##%2").arg(private_name).arg(my_name);
               tcpsocket->write(str.toUtf8());
               qDebug()<<"connect success!!!";

               time.start(100);
            });

    connect(&time,&QTimer::timeout,
            [=]()
            {
                time.stop();

                QString str=QString("私聊##%1##%2##进入私聊").arg(private_name).arg(my_name);
                tcpsocket->write(str.toUtf8());
            });

    connect(tcpsocket,&QTcpSocket::readyRead,
             [=]()
             {
                QByteArray array=tcpsocket->readAll();
                QString select=QString(array).section("##",0,0);

                if(select=="对方退出聊天")
                {                
                    ui->textBrowser->setTextColor(Qt::gray);
                    ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
                    ui->buttonSend->setEnabled(false);
                    ui->buttonFile->setEnabled(false);

                    ui->tableWidget->removeRow(1);

                    ui->textBrowser->append(array);

                }
                else if(select=="对方进入聊天")
                {
                    ui->textBrowser->setTextColor(Qt::gray);
                    ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
                    ui->buttonSend->setEnabled(true);
                    ui->buttonFile->setEnabled(true);

                    ui->tableWidget->insertRow(1);
                    QTableWidgetItem *Privatename= new QTableWidgetItem(private_name);
                    ui->tableWidget->setItem(1, 0, Privatename);
                    QTableWidgetItem *Privatestatus = new QTableWidgetItem("127.0.0.1");
                    ui->tableWidget->setItem(1, 1, Privatestatus);

                    ui->textBrowser->append(array);

                }
                else if(select=="文件信息")
                {
                    QString info=QString(array).section("##",1,1);
                    QString filename=QString(info).section("$$",0,0);
                    qDebug()<<QString(array);
                    qint64 filesize=QString(info).section("$$",1,1).toLongLong();
                    qDebug()<<"widget filesize"<<filesize<<" filename"<<filename;


                    int ret=QMessageBox::question(this,"文件传送","是否接受对方发来的文件？");

                    switch (ret)
                    {
                    case QMessageBox::Yes:{
                                            receiver * Receiver=new receiver(nullptr,filename,filesize,44444);
                                            Receiver->setWindowTitle("接受文件 from "+private_name);
                                            Receiver->setWindowIcon(this->windowIcon());
                                            Receiver->show();                                           

                                            QString str=QString("文件回复##%1##%2##接受文件").arg(private_name).arg(my_name);
                                            tcpsocket->write(str.toUtf8());

                                            break;
                                          }
                    case QMessageBox::No:{

                                            QString str=QString("文件回复##%1##%2##拒绝文件").arg(private_name).arg(my_name);
                                            tcpsocket->write(str.toUtf8());

                                            break;
                                         }
                    }

                }
                else
                {
                    ui->textBrowser->setTextColor(Qt::darkMagenta);
                    ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
                    times = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    ui->textBrowser->append(times+" "+private_name+":");

                    QString type=QString(array).section("$$",0,0);
                    if(type=="图片")
                    {
                        Imagename=QString(array).section("$$",1,1);

                        QString path = QString("<img src=\":/Image/%1\"/>").arg(Imagename);

                        ui->textBrowser->append("");
                        ui->textBrowser->insertHtml(path);

                        Imagename=nullptr;
                    }
                    else
                    {
                        ui->textBrowser->append("");
                        ui->textBrowser->insertHtml(QString(array));
                    }
                }
             });
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_buttonSend_clicked()
{
    ui->textBrowser->setTextColor(Qt::darkMagenta);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
    times = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textBrowser->append(times+" "+my_name+":");
    ui->textBrowser->append("");

    QString str;
    if(ui->textEditWrite->toPlainText()!="")
    {
        if(Imagename!=nullptr)
        {
            str=QString("私聊##%1##%2##图片$$%3").arg(private_name).arg(my_name).arg(Imagename);

            QString path = QString("<img src=\":/Image/%1\"/>").arg(Imagename);

            ui->textBrowser->insertHtml(path);

            Imagename=nullptr;
        }
        else
        {
            str=QString("私聊##%1##%2##%3").arg(private_name).arg(my_name).arg(ui->textEditWrite->toHtml());

            ui->textBrowser->insertHtml(ui->textEditWrite->toHtml());
        }

        tcpsocket->write(str.toUtf8());
        ui->textEditWrite->clear();
    }
}

void Widget::on_buttonDel_clicked()
{
    ui->textEditWrite->clear();
}

void Widget::on_buttonQuit_clicked()
{
    this->close();
}

void Widget::on_buttonImage_clicked()
{
    QString Imagepath= QFileDialog::getOpenFileName(this, tr("Select an image"),
                                      "E:/File/Mydemo/Project/Chatroom_project/Image", tr("Bitmap Files (*.bmp)\n"
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

void Widget::on_buttonBold_clicked(bool checked)
{
    if(checked)
        ui->textEditWrite->setFontWeight(QFont::Bold);
    else
        ui->textEditWrite->setFontWeight(QFont::Normal);
    ui->textEditWrite->setFocus();
}

void Widget::on_buttonUnderline_clicked(bool checked)
{
    ui->textEditWrite->setFontUnderline(checked);
    ui->textEditWrite->setFocus();
}

void Widget::on_buttonItalic_clicked(bool checked)
{
    ui->textEditWrite->setFontItalic(checked);
    ui->textEditWrite->setFocus();
}

void Widget::on_buttonColor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->textEditWrite->setTextColor(color);
        ui->textEditWrite->setFocus();
    }
}

void Widget::on_buttonHistory_clicked()
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

void Widget::on_buttonFile_clicked()
{
    class sender *Sender=new class sender(nullptr,my_name,private_name);
    Sender->setWindowTitle("传送文件 to "+private_name);
    Sender->setWindowIcon(this->windowIcon());
    Sender->show();

}

void Widget::on_buttonClear_clicked()
{
    ui->textBrowser->clear();
}

void Widget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->textEditWrite->setCurrentFont(f);
    ui->textEditWrite->setFocus();
}

void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->textEditWrite->setFontPointSize(arg1.toDouble());
    ui->textEditWrite->setFocus();
}

void Widget::on_textEditWrite_currentCharFormatChanged(const QTextCharFormat &format)
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

void Widget::closeEvent(QCloseEvent *)
{
    QString str=QString("私聊##%1##%2##退出私聊").arg(private_name).arg(my_name);
    tcpsocket->write(str.toUtf8());

//    QString sql=QString("update %1 set socket=-1 where id='%2'").arg(my_name).arg(private_name);
//    query.exec(sql);

    tcpsocket->disconnectFromHost();
    tcpsocket->close();
}


