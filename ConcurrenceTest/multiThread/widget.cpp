#include "widget.h"
#include <QDebug>
#include <windows.h>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    createView();
}

void Widget::createView()
{
    //添加界面
    //QPushButton *openThreadBtn = new QPushButton(QStringLiteral("打开线程"));
    //QPushButton *closeThreadBtn = new QPushButton(QStringLiteral("关闭线程"));
    QPushButton *testLoginBtn = new QPushButton(QStringLiteral("测试登录cgi"));
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(testLoginBtn);
    mainLayout->addStretch();
    connect(testLoginBtn,SIGNAL(clicked(bool)),this,SLOT(testLogin()));

    //connect(closeThreadBtn,SIGNAL(clicked(bool)),this,SLOT(closeThreadBtnSlot()));
    //connect(thread1,SIGNAL(finished()),this,SLOT(finishedThreadBtnSlot()));
}
void Widget::testLogin()
{

    const int num = 200;     // 由于是动态分配，这里不用const也可以
    // 这里的语法有点搞混了
    // 比如int* a = new int[10]; 代表a是一个int数组的首地址
    // 想要用new获得一个数组，不能使用int* a[10] = new int[10]; 这样的用法
    MyThread::setThreadTotalCount(num); // 将const给变量赋初值是没问题的
    MyThread* threads = new MyThread[num];
    for(int i = 0; i < num; ++i){
        threads[i].start();         // threads[i]相当于一个对象，而不是一个指向对象的指针
                                    // 这是由operator[]()的定义决定的
        qDebug()<<QStringLiteral("线程%1启动").arg(i)<<endl;
        MyThread::increaseStartedThreads();
    }

    // 等待所有线程执行完毕
    MyThread::waitAllThreadFinished();
    // 关闭线程
    for(int i = 0; i < num; ++i){
        //threads[i].closeThread(); isStop已经设置为true并且线程已经return了
        //threads[i].exit();
        threads[i].wait();

        qDebug()<<QStringLiteral("线程%1关闭").arg(i)<<endl;
    }
    delete [] threads;
    MyThread::setStartedThreadsZero();
}
void Widget::openThreadBtnSlot()
{
    //开启一个线程
    thread1->start();

    qDebug()<<QStringLiteral("主线程id：")<<QThread::currentThreadId();
}

void Widget::closeThreadBtnSlot()
{
    //关闭多线程
    thread1->closeThread();
    thread1->wait();
}

void Widget::finishedThreadBtnSlot()
{
    qDebug()<<QStringLiteral("完成信号finished触发");
}

Widget::~Widget(){}
