#ifndef FILELIST_H
#define FILELIST_H
#include "tools/fileinfo.h"
#include <QWidget>
#include "tools/uploadLayout.h"
#include <QTimer>
#include "tools/upload.h"
namespace Ui {
class filelist;
}

class filelist : public QWidget
{
    Q_OBJECT

public:
    explicit filelist(QWidget *parent = nullptr);
    ~filelist();
    void initListWidget();
    void showItems();
    void getFileList();     // 从服务器获取文件列表
    void getFileListJson(QByteArray data);
    QString getFileType(QString filename);
    QString getFileTypeIcon(QString type);
    void clearItems();
    void addUploadItem();       // 添加上传图标
    void uploadFiles();         // 打开一个窗口，选择需要上传的文件，添加到fList
    void checkTaskList();       // 定时检查上传和下载队列，并执行响应的操作
    void uploadFileToServer();  // 从fList取出一个文件，上传到服务器
    void realUploadFile(UploadFileInfo* file);  // 真正的上传一个文件
    QByteArray getMd5Json(const QString& md5, const QString& username, const QString& filename);
    QString getCode(const QByteArray& response);
private:
    Ui::filelist *ui;
    QList<FileInfo* > fList;

    // 定时器
    QTimer uploadTimer;
    QTimer downloadTimer;
};

#endif // FILELIST_H
