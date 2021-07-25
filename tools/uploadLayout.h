#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H
#include <QVBoxLayout>
#include <QWidget>
class UploadLayout{
public:
    static void setUploadLayout(QWidget* p);    // 设置布局
    static QLayout* getUploadLayout();     // 获取布局
private:
    UploadLayout(){}
    ~UploadLayout(){}
    static QLayout* m_layout;
    static QWidget* m_wg;
};

#endif // UPLOADLAYOUT_H
