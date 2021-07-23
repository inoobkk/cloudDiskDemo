#include "filelist.h"
#include "ui_filelist.h"
#include "tools/loginInfo.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>
#include <QIcon>
#include <QListWidgetItem>
filelist::filelist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filelist)
{
    ui->setupUi(this);
    // 初始化listWidget文件列表
    //initListWidget();
    // 添加右键菜单
    //addActionMenu();
}

filelist::~filelist()
{
    delete ui;
}

void filelist::initListWidget()
{
    ui->listWidget->setViewMode(QListView::IconMode);   // 设置显示图标模式
    ui->listWidget->setIconSize(QSize(80,80));          // 设置图标大小
    ui->listWidget->setGridSize(QSize(100,100));        // 设置item大小

    // 设置QListView大小改变时，图标的调整模式，默认是固定的，可以改成自动调整
    ui->listWidget->setResizeMode(QListView::Adjust);   // 自适应布局

    // 设置列表可以拖动，如果不想拖动，设置QListView::Static
    ui->listWidget->setMovement(QListView::Static);
    // 设置图标之间的间距，当setGridSize时，设置无效
    ui->listWidget->setSpacing(30);

    // listWidget右键菜单
    // 发出customContextMenuRequested信号
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // 点击列表中的上传文件图标
    connect(ui->listWidget, &QListWidget::itemPressed, this, [=](){
        //QString
    });
}



// 向服务器发起查询请求，获得当前登录用户的所有文件列表
void filelist::getFileList()
{
    qDebug()<<QStringLiteral("刷新列表中")<<endl;
    QString url = QString("http://%1:%2/myfiles").arg(LoginInfo::getIp()).arg(LoginInfo::getPort());
    qDebug() <<"url: "<<url<<endl;
    // 准备json数据
    QJsonObject obj;
    obj.insert("username", LoginInfo::getUsername());
    QJsonDocument doc = QJsonDocument(obj);
    // jsondoc->json string
    QByteArray data = doc.toJson();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(data.size()));
    request.setUrl(QUrl(url));
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkReply* reply = manager->post(request, data);
    if(NULL == reply){
        qDebug() << "reply is NULL"<<endl;
        return;
    }
    connect(reply, &QNetworkReply::readyRead,[=](){  // 为什么这里用finished才行，用readyRead不行？上面的url写错了
        QByteArray response = reply->readAll();
        qDebug()<<"response: "<<response<<endl;
        // 将字符串转为json格式
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if(!doc.isObject()){
            qDebug() << "doc is not an object"<< endl;
            showItems();
            return;
        }
        // 未获取到code：返回的是文件列表
        // 002: 云盘中没有上传的文件
        // 003: 获取文件列表失败，其他错误
        QJsonObject obj = doc.object();
        QJsonValue jvalue = obj.value("code");
        if(!jvalue.isObject()){
            qDebug() << QStringLiteral("该用户上传了文件")<<endl;
            getFileListJson(response);

        }
        showItems();




    });
}
// 解析响应数据
void filelist::getFileListJson(QByteArray data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(doc.isNull() || doc.isEmpty() || !doc.isObject()){
        qDebug() << "doc null" <<endl;
        return;
    }
    QJsonObject obj = doc.object();

    // 获取对应的文件数组
    QJsonArray array = obj.value("files").toArray();
    int size = array.size();
    qDebug()<<"size: "<<size<<endl;
    for(int i = 0; i < size; ++i){
        QJsonObject tmp = array[i].toObject();
        FileInfo* finfo = new FileInfo;
        finfo->fileName = tmp.value("filename").toString();
        qDebug()<<"filename: "<<finfo->fileName<<endl;
        finfo->fileType = getFileType(finfo->fileName);
        QString type = finfo->fileType;
        finfo->item = new QListWidgetItem(QIcon(getFileTypeIcon(type)), finfo->fileName);
        fList.append(finfo);
    }

}

QString filelist::getFileType(QString filename)
{
    QString res;
    int size = filename.size();
    int i = 0;
    while(i < size && filename[i] != '.'){
        ++i;
    }
    ++i;
    while(i < size){
        res += filename[i];
        ++i;
    }
    return res;
}

QString filelist::getFileTypeIcon(QString type)
{
    QString iconPath = QString(":fileType/fileType/%1.png").arg(type);
    return iconPath;
}

void filelist::clearItems()
{
    int n = ui->listWidget->count();
    for(int i = 0; i < n; ++i){
        QListWidgetItem* item = ui->listWidget->takeItem(0);
        delete item;
    }
}

void filelist::addUploadItem()
{
    QString iconPath =":/fileType/images/upload.png";
    ui->listWidget->addItem(new QListWidgetItem(QIcon(iconPath), QStringLiteral("上传文件")));
}
void filelist::showItems()
{
    clearItems();
    int n = fList.size();
    for(int i = 0; i < n; ++i){
        FileInfo *tmp = fList.at(i);
        QListWidgetItem *item = tmp->item;
        //list widget add item
        ui->listWidget->addItem(item);
    }
    addUploadItem();
}

