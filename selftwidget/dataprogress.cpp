#include "dataprogress.h"
#include "ui_dataprogress.h"
#include <QDebug>
dataprogress::dataprogress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataprogress)
{
    ui->setupUi(this);
}

dataprogress::~dataprogress()
{
    delete ui;
}

void dataprogress::setFileName(QString filename)
{
    ui->fileName->setText(filename + ":");
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
}

// 设置进度
void dataprogress::setProgress(int value, int max)
{
    qDebug()<<"set value"<<endl;
    // 这里顺序必须按照先写setMaximum，再写setValue
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
}
// 设置进度条的当前值value，最大值max

