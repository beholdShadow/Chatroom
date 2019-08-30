#ifndef DRAWER_H
#define DRAWER_H

#include <QTcpSocket>
#include <QWidget>
#include <QGroupBox>
#include <QToolButton>
#include <QToolBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QLineEdit>
#include <QTextCodec>
#include <QTabWidget>
#include <QEvent>

namespace Ui {
class drawer;
}

class drawer : public QWidget
{
    Q_OBJECT

public:
    explicit drawer(QWidget *parent,QString usrname);
    ~drawer();
    void open_database();
    void paintdrawer();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *);

private:
    Ui::drawer *ui;

    QTcpSocket *tcpsocket;

    QSqlDatabase db;

    QTimer *mytimer;

    QPoint P;

    QToolButton toolBtn[200];

    QToolButton groupBtn[20];

    qint16 my_port[10];

    int privateindex=0;
    int groupindex=0;

    QString myname;

    QLineEdit *SIGN;
    QLabel *NAME;
    QToolButton *image;
    QSpacerItem* vSpacer ;
    QGroupBox *privateBox;
    QVBoxLayout *layout;
    QGroupBox *groupBox;
    QVBoxLayout *layoutGroup;
    QToolBox *BOXone;
    QGroupBox *functionbox;
    QVBoxLayout *layfunction;
    QGroupBox *Messagebox;
    QVBoxLayout *laymessage;
    QToolBox *BOXtwo;
    QTabWidget *tabwidget;
    QVBoxLayout *layoutWidget;

};

#endif // DRAWER_H
