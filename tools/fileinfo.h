#ifndef FILEINFO_H
#define FILEINFO_H
#include <QString>
#include <QListWidgetItem>
class FileInfo{
public:
    QString fileName;   // 文件名
    QString fileType;   // 文件类型
    QListWidgetItem *item;
};

#endif // FILEINFO_H
