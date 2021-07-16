#include "titlewg.h"
#include "ui_titlewg.h"
#include <QMouseEvent>
TitleWg::TitleWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWg)
{
    m_parent = parent;
    ui->setupUi(this);
    ui->logo->setPixmap(QPixmap(":/upload/images/logo.jpg").scaled(40,40));
    // 如果ui里设置过了，这里不生效
    ui->wgtitle->setStyleSheet("color::RGB(255,255,255)");
    connect(ui->set, &QToolButton::clicked, [=](){
        // 发送自定义信号
        emit showSetWg();
    });
    // min
    connect(ui->min, &QToolButton::clicked, [=](){
        m_parent->showMinimized();
    });
    //close
    connect(ui->close, &QToolButton::clicked, [=](){
        // 发送自定义信号
        emit closeWindow();
    });

}

TitleWg::~TitleWg()
{
    delete ui;
}

void TitleWg::mouseMoveEvent(QMouseEvent *event)
{
    // 只允许左键拖动， buttons表示持续的动作，如长按
    if(event->buttons() & Qt::LeftButton){  // 检测到左键按下
        // 窗口跟着鼠标移动
        // 这里只能使当前的窗口移动
        //move(event->globalPos() - m_pt);
        // 使parent移动
        m_parent->move(event->globalPos() - m_pt);
    }
}

void TitleWg::mousePressEvent(QMouseEvent *event)
{
    // button表示短暂的一个事件
    if(event->button() == Qt::LeftButton){
        // 求出差值

        // m_pt = event->globalPos() - m_parent->geometry().topLeft();
        // 需要获得parent与鼠标之间的差值
        m_pt = event->globalPos() - m_parent->geometry().topLeft();
    }
}

