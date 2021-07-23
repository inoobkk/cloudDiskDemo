#ifndef FILELIST_H
#define FILELIST_H
#include "tools/fileinfo.h"
#include <QWidget>

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
    void addUploadItem();
private:
    Ui::filelist *ui;
    QList<FileInfo* > fList;
};

#endif // FILELIST_H
