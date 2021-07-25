#include "tools/upload.h"
#include "tools/loginInfo.h"
#include <QFileInfo>
#include <QDebug>
#include <QIODevice>
#include "tools/md5.h"
#include "tools/uploadLayout.h"
// 初始化static类型数据成员
QList<UploadFileInfo*> UploadFile::fList = QList<UploadFileInfo*>();

// 添加一个文件到上传列表
// -1: 文件太大
// -2: 文件已经在传输列表中
// -3: 打开文件失败
// -4： 获取传输列表布局失败
// 0:  成功
int UploadFile::appedUploadList(QString path)
{
    QFileInfo info(path);
    qint64 size = info.size();
    if(size > 30 * 1024 * 1204){
        qDebug()<<"file is too big"<<endl;
        return -1;
    }
    // 遍历fList，查看文件是否已经在上传队列中
    for(int i = 0; i != fList.size(); ++i){
        if(fList.at(i)->path == path){
            qDebug()<<"this file has been in the uploading list"<<endl;
            return -2;
        }
    }
    QFile* file = new QFile(path);      // 创建文件指针，指向一个打开的文件
    if(!file->open(QIODevice::ReadOnly)){
        qDebug()<<"failed to open the file"<<endl;
        delete file;
        file = NULL;
        return -3;
    }

    UploadFileInfo* newFile = new UploadFileInfo;
    newFile->fileName = info.fileName();
    newFile->size = size;
    newFile->path = path;
    newFile->isUploading = false;
    newFile->username = LoginInfo::getUsername();
    newFile->md5 = getFileMd5(*file);
    newFile->pfile = file;
    // 创建进度条
    dataprogress* p = new dataprogress;
    p->setFileName((newFile->fileName));
    newFile->dp = p;
    // 获取布局
    QVBoxLayout* layout = (QVBoxLayout*)UploadLayout::getUploadLayout();
    if(NULL == layout) return -4;
    layout->insertWidget(layout->count(), p);   // 将当前进度条添加到上传列表

    qDebug()<<newFile->fileName.toUtf8().data()<<"is added to uploading lsit"<<endl;
    fList.append(newFile);

    return 0;

}

bool UploadFile::isEmpty()
{
    return fList.isEmpty();
}

bool UploadFile::isUploading()
{
    for(int i = 0; i != fList.size();++i){
        if(fList.at(i)->isUploading == true){   // 是否有文件正在上传
            return true;
        }
    }
    return false;
}

// 拿出一个文件信息，用来上传
UploadFileInfo *UploadFile::getTask()
{
    UploadFileInfo* tmp = fList.at(0);
    fList.at(0)->isUploading = true;    // 将该文件设置为正在上传
    return tmp;
}

// 删除已经上传完成的进度条，这里保证调用该函数时isuploading为true的文件已经上传完成
void UploadFile::dealUploadTask()
{
    for(int i = 0; i != fList.size(); ++i){
        if(fList.at(i)->isUploading == true){
            UploadFileInfo* tmp = fList.takeAt(i);
            // 获取布局
            QLayout* layout = UploadLayout::getUploadLayout();
            layout->removeWidget(tmp->dp);

            // 关闭打开的文件指针
            QFile* file = tmp->pfile;
            file->close();
            delete file;
            delete tmp->dp;
            delete  tmp;
            return;
        }
    }
}

