#ifndef LOGIN_H
#define LOGIN_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDialog>
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;
    //Common m_cm;
protected:
    void paintEvent(QPaintEvent* event);
private slots:
    void on_reg_btn_2_clicked();

    // 保存配置文件信息
    void saveWebServerInfo(QString ip, QString port, QString path);
    // 将用户输入的注册信息转换成json对象
    QByteArray getRegJson(QString username, QString pwd, QString mail);
    // 初始化登录界面的函数,读配置文件
    void on_reg_btn_clicked();
    QString getJsonWebResponse(QByteArray jsondata);
};

#endif // LOGIN_H
