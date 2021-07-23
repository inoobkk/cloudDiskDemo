#include "tools/loginInfo.h"

QString LoginInfo::ip = "";
QString LoginInfo::port = "";
QString LoginInfo::username = "";


QString LoginInfo::getIp()
{
    return ip;
}

QString LoginInfo::getPort()
{
    return port;
}

QString LoginInfo::getUsername()
{
    return username;
}

void LoginInfo::setInfo(QString ip_, QString port_, QString username_)
{
    ip = ip_;
    port = port_;
    username = username_;
}
