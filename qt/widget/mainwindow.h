#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void render(QPainter *p);
public slots:
    void onTimeChange();

private slots:
    void on_today_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H
