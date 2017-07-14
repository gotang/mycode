#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>

class Point {
public:
    Point(double x0, double y0, double z0) {
        x = x0;
        y = y0;
        z = z0;
    }
    double x;
    double y;
    double z;

};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->coord->setEnabled(false);
    setWindowFlags(Qt::WindowFullscreenButtonHint);


    connect(ui->A1toA0, SIGNAL(textEdited(QString)), this, SLOT(calculate()));
    connect(ui->toA0, SIGNAL(textEdited(QString)), this, SLOT(calculate()));
    connect(ui->toA1, SIGNAL(textEdited(QString)), this, SLOT(calculate()));
    connect(ui->clearButton, SIGNAL(clicked(bool)), this, SLOT(onClearButtonClick()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::calculate()
{
    qDebug() << "calculate";
    Point A0(0, 0, 0);
    Point A1(ui->A1toA0->text().toDouble(), 0, 0);
    double dA0 = ui->toA0->text().toDouble();
    double dA1 = ui->toA1->text().toDouble();

    if(A1.x == 0 || dA0 == 0 || dA1 == 0) {

        return;
    }

    Point x(0, 0, 0);
    /*
     *(A0->x - x)^2 + (A0->y - y)^2 = dA0^2
     *(A1->x - x)^2 + (A1->y - y)^2 = dA1^2
     *A0(0.00, 0.00)
     *A1(A1->x,0.00)
    */
    x.x = (dA0 * dA0 - dA1 * dA1 + A1.x * A1.x) /(2 * A1.x);
    x.y = sqrt(dA0 * dA0 - x.x * x.x);
    x.z = 0;

    QString out = QString("(%1, %2, %3)").arg(x.x).arg(x.y).arg(x.z);
    qDebug() << out;
    ui->coord->setText(out);
}

void MainWindow::onClearButtonClick()
{
    ui->toA0->clear();
    ui->toA1->clear();
    ui->coord->clear();
}

