#include "uploadLayout.h"

QLayout* UploadLayout::m_layout = NULL;
QWidget* UploadLayout::m_wg = NULL;
void UploadLayout::setUploadLayout(QWidget* p){
    m_wg = new QWidget(p);
    QLayout* layout = p->layout();
    layout->addWidget(m_wg);
    layout->setContentsMargins(0,0,0,0);        // 设置边距
    QVBoxLayout* vlayout = new QVBoxLayout;     // 垂直布局
    m_wg->setLayout(vlayout);
    vlayout->setContentsMargins(0,0,0,0);
    m_layout = vlayout;

    // 添加弹簧
    vlayout->addStretch();  // 添加一个弹簧


}

QLayout* UploadLayout::getUploadLayout(){
    return m_layout;
}
