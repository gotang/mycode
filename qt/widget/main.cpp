#if 1
#include "mainwindow.h"
#include "netaddress.h"
#include <QApplication>
#include <QDebug>>

class MyApplicatioin :public QApplication {
   // Q_OBJECT
public:
    explicit MyApplicatioin(int argc, char *argv[]) :
        QApplication(argc, argv)
    {
    }
    virtual ~MyApplicatioin() {
    }
};


int main(int argc, char *argv[])
{
    MyApplicatioin a(argc, argv);
#if 0
    MainWindow w;
    w.show();
#endif
#if 1
    NetAddress n;
    n.show();
#endif
    return a.exec();
}

#else


#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *window = new QWidget;
    window->setWindowTitle("Enter Your Age");

    QSpinBox *spinBox = new QSpinBox;
    QSlider *slider = new QSlider(Qt::Horizontal);
    spinBox->setRange(0, 130);
    slider->setRange(0, 130);

    QObject::connect(spinBox, SIGNAL(valueChanged(int)),
                     slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)),
                     spinBox, SLOT(setValue(int)));
    spinBox->setValue(35);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(spinBox);
    layout->addWidget(slider);
    window->setLayout(layout);

    window->show();

    return app.exec();
}

#endif
