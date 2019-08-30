#ifndef GROUP_H
#define GROUP_H

#include <QWidget>
#include <QUdpSocket>
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
#include <QImageReader>
#include "groupsend.h"
#include "receiver.h"

namespace Ui {
class group;
}

class group : public QWidget
{
    Q_OBJECT

public:
    explicit group(QWidget *parent,QString usrname,qint16 port,QString groupname,QSqlDatabase mydb);
    ~group();

    void open_database();
protected:
     void closeEvent(QCloseEvent *);

private slots:
    void on_buttonSend_clicked();

    void on_buttonDel_clicked();

    void on_buttonQuit_clicked();

    void on_buttonBold_clicked(bool checked);

    void on_buttonUnderline_clicked(bool checked);

    void on_buttonItalic_clicked(bool checked);

    void on_buttonFile_clicked();

    void on_buttonHistory_clicked();

    void on_buttonClear_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_textEditWrite_currentCharFormatChanged(const QTextCharFormat &format);

    void on_buttonColor_clicked();

    void on_buttonForbid_clicked();

    void on_buttonKick_clicked();

    void on_buttonUnforbid_clicked();

    void on_buttonImage_clicked();

private:
    Ui::group *ui;
    QUdpSocket *udpsocket;

    quint16 my_port;
    QString my_name;
    QString my_groupname;
    QTimer *timer;
    QColor color;
    QSqlDatabase db;
    QString time;
    bool isBoss;

    QString Imagename=nullptr;
};

#endif // GROUP_H
