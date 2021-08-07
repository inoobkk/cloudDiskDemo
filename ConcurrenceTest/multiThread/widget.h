#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <mythread.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMutex>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void createView();

private slots:
    void openThreadBtnSlot();
    void closeThreadBtnSlot();
    void finishedThreadBtnSlot();
    void testLogin();
//    void testBtnSlot();

private:
    QVBoxLayout *mainLayout;
    MyThread *thread1;
};

#endif // WIDGET_H
