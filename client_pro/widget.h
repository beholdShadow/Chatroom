#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTimer>
#include <QTextCharFormat>
#include <QCloseEvent>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QColorDialog>
#include <QFileDialog>
#include <QTextCodec>
#include "sender.h"
#include "receiver.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent,QString usrname,QString privatename,QSqlDatabase mydb);
    ~Widget();
protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_buttonSend_clicked();

    void on_buttonDel_clicked();

    void on_buttonQuit_clicked();   

    void on_buttonBold_clicked(bool checked);

    void on_buttonUnderline_clicked(bool checked);

    void on_buttonItalic_clicked(bool checked);

    void on_buttonColor_clicked();

    void on_buttonHistory_clicked();

    void on_buttonFile_clicked();

    void on_buttonClear_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_textEditWrite_currentCharFormatChanged(const QTextCharFormat &format);


    void on_buttonImage_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket *tcpsocket;
    QString private_name;
    QString my_name;
    QSqlDatabase db;
    QTimer time;
    QColor color;
    QString times;

    QString Imagename=nullptr;
};

#endif // WIDGET_H
