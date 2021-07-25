#ifndef MD5_H
#define MD5_H
#include <QString>
#include <QFile>
#include <QCryptographicHash>
QString getFileMd5(QFile& localFile){
    QCryptographicHash ch(QCryptographicHash::Md5);

        quint64 totalBytes = 0;
        quint64 bytesWritten = 0;
        quint64 bytesToWrite = 0;
        quint64 loadSize = 1024 * 4;
        QByteArray buf;

        totalBytes = localFile.size();
        bytesToWrite = totalBytes;

        while (1)
        {
            if(bytesToWrite > 0)
            {
                buf = localFile.read(qMin(bytesToWrite, loadSize));
                ch.addData(buf);
                bytesWritten += buf.length();
                bytesToWrite -= buf.length();
                buf.resize(0);
            }
            else
            {
                break;
            }

            if(bytesWritten == totalBytes)
            {
                break;
            }
        }

        //localFile.close();
        QByteArray md5 = ch.result();
        return md5.toHex();
}
#endif // MD5_H
