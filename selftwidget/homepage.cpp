#include "homepage.h"
#include "ui_homepage.h"

homePage::homePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::homePage)
{
    ui->setupUi(this);
    // 去边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 捕获titleWg发送的信号并做出响应的响应
    connect(ui->titleWg, &buttonGroup::filelist_clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->filelist_page);
        // 获取当前用户的文件列表
    });
    connect(ui->titleWg, &buttonGroup::sharelist_clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->sharelist_page);
        // 获取共享的文件列表
    });
    connect(ui->titleWg, &buttonGroup::downloadrank_clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->downloadrank_page);
        // 获取下载量
    });
    connect(ui->titleWg, &buttonGroup::transferlist_clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->transferlist_page);
        // 获取传输列表
    });
    connect(ui->titleWg, &buttonGroup::switchuser_clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->switchuser_page);
        // 退出
    });
}

homePage::~homePage()
{
    delete ui;
}
