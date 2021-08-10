#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "common/common.h"
#include "selftwidget/homepage.h"
#include "tools/loginInfo.h"
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    // 去除边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    // 设置默认的ip地址
    ui->address->setText("101.132.162.195");
    ui->port->setText("80");
    // 设置当前窗口所在的所有字体大小,如果已经在ui里设置过，这里不生效
    //this->setFont(QFont("华文彩云",16, QFont::Bold, false));

    // tileWg信号处理
    connect(ui->title_wg, &TitleWg::closeWindow, [=](){
        // 判断当前stackWidget显示的页面
        if(ui->stackedWidget->currentWidget() == ui->set_page){
            // 当前是设置页面，切换到login页面
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            // 清除控件数据
        }
        else if(ui->stackedWidget->currentWidget() == ui->reg_page){
            // 当前是注册界面，切换到登录
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            // 清除控件数据
            //
        }
        else{
            // 当前是登录页面，此时再点×号就是退出
            this->close();
        }
    });

    connect(ui->title_wg, &TitleWg::showSetWg, [=](){
        ui->stackedWidget->setCurrentWidget(ui->set_page);
    });
    connect(ui->toReg, &QToolButton::clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->reg_page);
    });
}

// 正则表达式的用法
// 数字字母下划线
// 对应的类QRegexp reg(要使用的正则表达式)
// reg.exactMatch(需要验证的字符串，也就是用户输入的用户名)
// exactMatch的返回值为true则输入符合要求，false不符合要求

// json字符串解析
// 首先需要一个JsonDocument类：
Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // 给窗口画背景图
    QPainter p(this);   // 画到this（当前窗口）上, 加了this参数表明绘图作用在局部，不影响全局的其他窗口
    QPixmap pixmap(":/upload/images/background.jpg");
    p.drawPixmap(0,0,this->width(),this->height(), pixmap);     // 从左上角画到右下角
}




void Login::on_reg_btn_2_clicked()
{
    // 获取控件数据
    QString ip = ui->address->text();
    QString port = ui->port->text();
    // 使用正则表达式进行数据校验
    QRegExp exp(IP_REG);
    if(!exp.exactMatch(ip)){
        QMessageBox::warning(this,"warning","incorrect ip form");
        ui->address->clear();
        // 设置焦点
        ui->address->setFocus();

        // 端口校验
        // 重新设置正则表达式
        exp.setPattern(PORT_REG);
        if(!exp.exactMatch(port)){

        }
        // 将输入的配置信息保存到配置文件


    }
}

void Login::saveWebServerInfo(QString ip, QString port, QString path)
{
    // 以下代码未完成
    /*
    // 先读文件
    QFile file(path);

    bool isOpen = file.open(QFile::ReadOnly);
    if(isOpen == false){
        // 文件不存在
    }
    QByteArray data = file.readAll();
    //  先读原来的配置文件
    QJsonDocument doc = QJsonDocument::fromJson(data);
    // 读取login信息
    if(!doc.isObject())
    {
        // 文件不存在
        // 新建一个文件
        return;
    }
    // 字符串转为json对象
    QJsonObject obj = doc.object();
    QJsonObject loginobj = obj.value("login");
    // 取出子对象数据
    // pwd remeber user
    QMap<QString, QVariant> logininfo;
    //logininfo.insert("pwd", xxx);
    //logininfo.insert("user", xxx);
    //logininfo.insert("remember", xxx);

    // 去除图片路径信息
    QJsonObject pathobj = obj.value("login");
    QMap<QString, QVariant> pathinfo;
    //pathinfo.insert("path", xxx);
    // 存储web服务器配置信息
    QMap<QString, QVariant> webinfo;
    webinfo.insert("ip", QVariant(ip));
    webinfo.insert("port",QVariant(port));

    QMap<QString, QVariant> info;
    info.insert("login", logininfo);
    info.insert("type_path", pathinfo);
    info.insert("web_server", logininfo);

    doc = QJsonDocument::fromVariant(info);

    // 将json转换成字符串，保存到文件
    data = doc.toJson();
    // 写文件
    */

}

QByteArray Login::getRegJson(QString username, QString pwd, QString mail)
{
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("passwd", pwd);
    obj.insert("email", mail);
    // obj -> jsondoc
    QJsonDocument doc = QJsonDocument(obj);
    return doc.toJson();
}
// 并没有找到调用此函数的，该函数是如何被调用的？通过槽实现的
void Login::on_reg_btn_clicked()
{
    // bug:1.当所有的注册信息为空时也可以成功注册
    qDebug() << "reg"<<endl;
    QString ip = ui->address->text();   //  获得ip地址
    QString port = ui->port->text();    // 获得端口号
    QString username = ui->name_reg->text();
    QString password = ui->pwd_reg->text();
    QString confirm_password = ui->confirm_pwd_reg->text();
    QString email = ui->mail_reg->text();
    // 输入校验
    QRegExp usernameExp(USER_REG);
    QRegExp pwdExp(PASSWD_REG);
    QRegExp emailExp(EMAIL_REG);
    QRegExp ipReg(IP_REG);
    QRegExp portReg(PORT_REG);
    if(!ipReg.exactMatch(ip) || !portReg.exactMatch(port)){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("ip或端口格式错误"));
        return;
    }
    if(!usernameExp.exactMatch(username) || !pwdExp.exactMatch(password)){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("用户名或密码输入格式错误"));
        return;
    }
    if(!emailExp.exactMatch(email)){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("邮箱格式错误"));
        return;
    }
    if(confirm_password != password){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("两次输入的密码不一致"));
        return;
    }

    // 将数据转成json格式
    QMap<QString, QVariant> jsonMap;
    jsonMap.insert("username", username);
    jsonMap.insert("password", password);
    jsonMap.insert("email", email);
    QJsonDocument doc = QJsonDocument::fromVariant(jsonMap);
    if(!doc.isObject()){
        qDebug()<<"line184: doc is null"<<endl;
        return;
    }
    QByteArray jsonValue = doc.toJson();
    qDebug() << "data: " << jsonValue << endl;
    QNetworkRequest request;
    QString url = QString("http://%1:%2/reg").arg(ip).arg(port);
    qDebug() << "url: " << url << endl;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(jsonValue.size()));
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkReply* reply = manager->post(request, jsonValue);
    qDebug() << "post"<<endl;
    // 处理响应数据
    connect(reply, &QNetworkReply::readyRead, [=](){
        qDebug()<<"connect"<<endl;
        QByteArray response = reply->readAll();
        qDebug()<<"response: "<< response<<endl;

        // 解析json字符串
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if(!doc.isObject()){
            qDebug()<<"line204: error type of response"<<endl;
            return;
        }
        QJsonObject obj = doc.object();
        QString status = obj.value("code").toString();
        /*
        注册 - server端返回的json格式数据：
            成功:         {"code":"002"}
            该用户已存在：  {"code":"003"}
            失败:         {"code":"004"}
        */
        if("002" == status){
            qDebug()<<"注册成功"<<endl;
            QMessageBox::information(this, QStringLiteral("注册成功"),QStringLiteral("注册成功,请登录"));
            ui->login_username->setText(username);
            ui->login_passwd->setText(password);
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            // 清除填写的注册信息
            ui->name_reg->clear();
        }
        else if("003" == status){
            qDebug()<<QStringLiteral("注册失败,用户已经存在")<<endl;
            ui->name_reg->clear();
            QMessageBox::warning(this, QStringLiteral("注册失败"), QStringLiteral("用户已经存在"));

        }
        else{
            qDebug()<<QStringLiteral("注册失败,未知原因")<<endl;
            ui->name_reg->clear();
            QMessageBox::warning(this, QStringLiteral("注册失败"), QStringLiteral("注册失败"));
        }
        //delete reply;
        //free(reply);    // 为什么释放replay会异常终止？
        //delete manager;   // 这里也是释放manager
    });
    //delete manager;   不能把delete manager放在这里,这是一个bug
}

QString Login::getJsonWebResponse(QByteArray jsondata)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsondata);
    if(!doc.isObject()){
        return "004";
    }
    // 字符串转为json对象
    QJsonObject obj = doc.object();
    QJsonValue statusobj = obj.value("code");
    // 取出子对象数据
    return statusobj.toString();
}

void Login::on_login_btn_clicked()
{
    // 获取用户名和密码
    QString username = ui->login_username->text();
    QString password = ui->login_passwd->text();
    // 获取服务器ip地址和端口号
    QString ip = ui->address->text();
    QString port = ui->port->text();
    QRegExp nameExp(USER_REG);
    QRegExp pwdExp(PASSWD_REG);
    QRegExp ipExp(IP_REG);
    QRegExp portExp(PORT_REG);
    if(!ipExp.exactMatch(ip) || !portExp.exactMatch(port)){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("ip或端口格式错误"));
        return;
    }
    if(!nameExp.exactMatch(username) || !pwdExp.exactMatch(password)){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("用户名或密码输入格式错误"));
        return;
    }
    // 跳转到注册页面的功能写在了Login构造函数中，通过connect实现

    // 将用户名和和密码转换成Json数据格式
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("password", password);
    // jsonobj->jsondoc;
    QJsonDocument doc = QJsonDocument(obj);
    QByteArray loginJson = doc.toJson();
    // 准备发送数据
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(loginJson.size()));
    // 设置url
    QString url = QString("http://%1:%2/login").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    QNetworkAccessManager* m = new QNetworkAccessManager;
    QNetworkReply* reply = m->post(request, loginJson);
    // 等待响应
    connect(reply, &QNetworkReply::readyRead, [=](){
           QByteArray response = reply->readAll();
           // 将得到的响应字符串转为json格式，以提取状态码
           qDebug()<<response<<endl;
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
               // 登录成功，跳转到文件列表页面
               // to do 1.添加token验证码
               // 设置登录信息
               LoginInfo::setInfo(ip, port, username);
               qDebug()<<QStringLiteral("登录成功")<<endl;
               this->hide();
               homePage* home = new homePage;   // 这里好像没办法释放了
               home->showHomepage();
           }
           else if("003" == status){
               QMessageBox::warning(this, QStringLiteral("登录失败"),QStringLiteral("用户名不存在"));
               // 这里不进行用户名清空，方便用户检查
           }
           else if("002" == status){
               QMessageBox::warning(this, QStringLiteral("登录失败"),QStringLiteral("用户名或密码不正确"));
           }
           else{
               QMessageBox::warning(this, QStringLiteral("登录失败"),QStringLiteral("登陆失败"));
           }
           // 释放资源
    });
}
