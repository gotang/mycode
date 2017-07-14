#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDesktopServices>
#include <QTimer>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() <<"MainWindow";
    m_timer = new QTimer();
    m_timer->setSingleShot(false);
    m_timer->setInterval(1*1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeChange()));
    m_timer->start();
    on_today_clicked();
    ui->comboBox->addItem(QString(tr("Item 1")));
    ui->comboBox->addItem(QString(tr("Item 2")));
    ui->comboBox->setEnabled(false);


}

void MainWindow::render(QPainter *p)
 {
     static const QPoint hourHand[3] = {
         QPoint(7, 8),
         QPoint(-7, 8),
         QPoint(0, -40)
     };
     static const QPoint minuteHand[3] = {
         QPoint(7, 8),
         QPoint(-7, 8),
         QPoint(0, -70)
     };

     QColor hourColor(127, 0, 127);
     QColor minuteColor(0, 127, 127, 191);

     p->setRenderHint(QPainter::Antialiasing);
     p->translate(width() / 2, height() / 2);

     int side = qMin(width(), height());
     p->scale(side / 200.0, side / 200.0);

     QTime time = QTime::currentTime();

     p->save();
     p->rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
     p->drawConvexPolygon(hourHand, 3);
     p->restore();


     p->save();
     p->rotate(6.0 * (time.minute() + time.second() / 60.0));
     p->drawConvexPolygon(minuteHand, 3);
     p->restore();



     p->setPen(minuteColor);

     for (int j = 0; j < 60; ++j) {
         if ((j % 5) != 0)
             p->drawLine(92, 0, 96, 0);
         p->rotate(6.0);
     }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onTimeChange() {
    QDateTime date = QDateTime::currentDateTime();
    QString time = date.time().toString("HH:mm:ss");
    qDebug()<<"time is " << time;
    ui->lcdNumber->display(time);
    ui->progressBar->setValue(date.time().second()%10);
}


void MainWindow::on_today_clicked()
{
    QPainter * painter = new QPainter();

    render(painter);
}
