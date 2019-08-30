#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void initTcpsocket();
    void thread_deal(int i);
    void new_connect();
    void open_database();

private:
    Ui::Widget *ui;



};

#endif // WIDGET_H
