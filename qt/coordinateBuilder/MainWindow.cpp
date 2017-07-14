#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    QStringList tableHeader;
    tableHeader <<"No." << "toA0" << "toA1" <<"X\n(m)" << "Y\n(m)" << "Z\n(m)";

    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}
