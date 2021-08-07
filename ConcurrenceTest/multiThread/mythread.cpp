#include "mythread.h"
#include <QDebug>
#include <QMutex>
bool MyThread::allThreadIsReady = false;
int MyThread::finishedThread = 0;
int MyThread::totalThreadCount = 0;     // 没办法，必须定义，设置在后面
int MyThread::startedThread = 0;
int MyThread::successCount = 0;
MyThread::MyThread()
{
    isStop = false;
}

void MyThread::closeThread()
{
    isStop = true;
}

void MyThread::waitAllThreadReady()
{
    while(startedThread != totalThreadCount){
        ;
    }
    allThreadIsReady = true;
}


void MyThread::setThreadTotalCount(int n)
{
    totalThreadCount = n;
}



void MyThread::waitAllThreadFinished()
{
    while(finishedThread != totalThreadCount){
        // 忙等待
    }
    finishedThread = 0;     // 重新置为0
}

// 关闭所有线程后执行的操作
void MyThread::setStartedThreadsZero()
{
    qDebug()<<"total request: "<<totalThreadCount<<"success: "<<successCount<<endl;
    startedThread = 0;
    allThreadIsReady = false;
    successCount = 0;
}

void MyThread::increaseStartedThreads()
{
    ++startedThread;
    if(startedThread == totalThreadCount){
        allThreadIsReady = true;
    }
}

void MyThread::tryLogin()
{
    // 登录信息
    QString username = QString("1");
    QString password = QString("2");
    QString ip = QString("101.132.162.195");
    QString port = QString("80");

    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("password", password);
    QJsonDocument doc = QJsonDocument(obj);
    QByteArray loginJson = doc.toJson();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(loginJson.size()));
    QString url = QString("http://%1:%2/login").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    QNetworkAccessManager* m = new QNetworkAccessManager;
    QNetworkReply* reply = m->post(request, loginJson);

    connect(reply, &QNetworkReply::readyRead, [=](){
        QByteArray response = reply->readAll();
        // 将得到的响应字符串转为json格式，以提取状态码
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if(!doc.isObject()){
            qDebug()<<"line296: error type of response"<<endl;
            return;
        }
        QJsonObject obj = doc.object();
        QString status = obj.value("code").toString();
        // 判断状态码
        // 001：登录成功
        // 003: 用户不存在
        // 002: 密码错误
        // 004: 失败
        if("001" == status){
            successCount += 1;
        }else{
            qDebug()<<"status: "<<status<<endl;
        }
        m->deleteLater();
        reply->deleteLater();
        exit();
    });
    exec();
}

void MyThread::run()
{

    while (1)
    {
        if(isStop){
            ++finishedThread; // 执行完一个线程

            return;
        }
        if(allThreadIsReady){
            qDebug()<<"a thread is started"<<endl;
            // 所有线程就绪，执行下面的代码
            tryLogin();
            // 允许不同时退出
            isStop = true;

        }


    }
}
