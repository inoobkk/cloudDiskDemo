#ifndef LOGININFO_H
#define LOGININFO_H
#include <QString>
/*----------------说明----------------
 * 该类用来记录登录的信息，将该类设置为静态类，
 * 即整个代码中只能包含一份登录信息
*/
class LoginInfo{
public:
    //LoginInfo* getInstance();
    static QString getIp();
    static QString getPort();
    static QString getUsername();
    static void setInfo(QString ip_, QString port_, QString username_);
private:
    // 不允许创建对象
    LoginInfo(){}
    ~LoginInfo(){}
    static QString ip;
    static QString port;
    static QString username;


};

#endif // LOGININFO_H
