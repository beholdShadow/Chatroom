#ifndef DIALOG_H
#define DIALOG_H

#include <QTcpSocket>
#include <QMessageBox>
#include <QMovie>
#include <QMouseEvent>
#include <QHostAddress>
#include <QPoint>
#include "login.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void on_buttonQuit_clicked();

    void on_buttonEnter_clicked();

    void on_buttonLogin_clicked();

private:
    Ui::Dialog *ui;

    QTcpSocket *tcpsocket=nullptr;

    QMovie GIFLOGIN;

    Login *login;

    QPoint p;

    void revsignal(QString name);
};
#endif // DIALOG_H
