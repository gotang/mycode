#ifndef NETADDRESS_H
#define NETADDRESS_H

#include <QWidget>

namespace Ui {
class NetAddress;
}

class NetAddress : public QWidget
{
    Q_OBJECT

public:
    explicit NetAddress(QWidget *parent = 0);
    ~NetAddress();

private slots:
    void on_pushButton_clicked();

private:
    Ui::NetAddress *ui;
};

#endif // NETADDRESS_H
