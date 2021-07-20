#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>
#include <QMouseEvent>
namespace Ui {
class buttonGroup;
}

class buttonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit buttonGroup(QWidget *parent = nullptr);
    ~buttonGroup();

private:
    Ui::buttonGroup *ui;
    QPoint m_pt;
    QWidget* m_parent;
protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
private slots:
    void on_min_clicked();
    void on_max_clicked();
    void on_close_clicked();
signals:
    void filelist_clicked();
    void sharelist_clicked();
    void downloadrank_clicked();
    void transferlist_clicked();
    void switchuser_clicked();

};

#endif // BUTTONGROUP_H
