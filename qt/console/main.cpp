#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QObject>
#include "MyApplication.h"

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    qDebug() << "main thread:" << QThread::currentThreadId();
    return app.QApplication::exec();
}

