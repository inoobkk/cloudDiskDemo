#ifndef UPLOAD_H
#define UPLOAD_H
#include <QString>
#include <QFile>
#include "selftwidget/dataprogress.h"
#include <QList>
struct UploadFileInfo{
    QString fileName;       // 上传文件名
    QString username;       // 上传的用户名
    QString md5;            // 上传文件的md5值
    QFile* pfile;           // 文件指针
    QString path;           // 文件路径
    qint64 size;            // 文件大小
    dataprogress* dp;       // 上传的进度条
    bool isUploading;       // 该文件是否正在上传

};
// UploadFile设置为单例模式
class UploadFile{
public:
    // 将文件添加到上传列表中
    // 参数：path 上传文件路径
    // 返回值：成功： 0 失败：-1 文件大于30M -2：上传的文件已经在上传队列中
    // -3：打开文件失败 -4：获取布局失败
    static int appedUploadList(QString path);
    // 上传队列是否为空
    static bool isEmpty();
    // 是否有文件正在上传
    static bool isUploading();

    // 取出第0个上传任务
    static UploadFileInfo* getTask();
    // 删除上传完成的任务
    void dealUploadTask();
    // 清空上传列表
    void clearList();
private:
    UploadFile(){}      // 不允许创建对象
    ~UploadFile(){}

    static QList<UploadFileInfo* > fList;   // 上传文件任务列表

};

#endif // UPLOAD_H
