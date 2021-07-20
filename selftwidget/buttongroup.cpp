#include "buttongroup.h"
#include "ui_buttongroup.h"
#include <QtDebug>
buttonGroup::buttonGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::buttonGroup)
{
    m_parent = parent;
    ui->setupUi(this);
    // 设置图标名称
    ui->userinfo->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->filelist->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->sharelist->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->downloadrank->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->switchuser->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->transferlist->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->userinfo->setText(QStringLiteral("inoobkk"));
    ui->filelist->setText(QStringLiteral("我的文件"));
    ui->sharelist->setText(QStringLiteral("共享文件"));
    ui->downloadrank->setText(QStringLiteral("下载排行"));
    ui->switchuser->setText(QStringLiteral("切换用户"));
    ui->transferlist->setText(QStringLiteral("切换用户"));

    // 向parent发送信号
    connect(ui->filelist, &QToolButton::clicked, [=](){
        emit filelist_clicked();
    });
    connect(ui->sharelist, &QToolButton::clicked, [=](){
        emit sharelist_clicked();
    });
    connect(ui->downloadrank, &QToolButton::clicked, [=](){
        emit downloadrank_clicked();
    });
    connect(ui->transferlist, &QToolButton::clicked, [=](){
        emit transferlist_clicked();
    });
    connect(ui->switchuser, &QToolButton::clicked, [=](){
        emit switchuser_clicked();
    });
}
buttonGroup::~buttonGroup()
{
    delete ui;
}

void buttonGroup::mouseMoveEvent(QMouseEvent *event)
{
    qDebug()<<"move"<<endl;
    if(event->buttons() & Qt::LeftButton){
        m_parent->move(event->globalPos() - m_pt);
    }
}

void buttonGroup::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"press"<<endl;
    if(event->button() == Qt::LeftButton){
        m_pt = event->globalPos() - m_parent->geometry().topLeft();
    }
}

void buttonGroup::on_min_clicked()
{
    m_parent->showMinimized();
}

void buttonGroup::on_max_clicked()
{
    // to do：缩放
    if(m_parent->isMaximized()){
        m_parent->showNormal();
    }
    else{
        m_parent->showMaximized();
    }

}


void buttonGroup::on_close_clicked()
{
    m_parent->close();
}

