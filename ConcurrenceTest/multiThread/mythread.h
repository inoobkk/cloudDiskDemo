#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class MyThread : public QThread
{
public:
    MyThread();
    void closeThread();
    static void waitAllThreadReady();   // 设置为static，不需要通过对象调用
    static void setThreadTotalCount(int n);
    static void waitAllThreadFinished();
    static void setStartedThreadsZero();
    static void increaseStartedThreads();   // 每启动一个线程值+1
    void tryLogin();                    // 尝试登录云盘，true登录成功，false登录失败
protected:
    virtual void run();

private:
    volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
    static bool allThreadIsReady;
    static int finishedThread;
    static int totalThreadCount;
    static int startedThread;   // 记录已经启动的线程
    static int successCount;    // 成功登录计数
};

#endif // MYTHREAD_H
