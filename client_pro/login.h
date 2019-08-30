#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QTcpSocket>
#include <QMessageBox>
#include <QMouseEvent>
#include <QHostAddress>
#include <QPoint>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent);
    ~Login();
signals:
    void mysignal(QString name);
protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Login *ui;
    QTcpSocket *mytcpsocket;
    QPoint p;
};

#endif // LOGIN_H
