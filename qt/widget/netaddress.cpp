#include "netaddress.h"
#include "ui_netaddress.h"

NetAddress::NetAddress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetAddress)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

NetAddress::~NetAddress()
{
    delete ui;
}

void NetAddress::on_pushButton_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}
