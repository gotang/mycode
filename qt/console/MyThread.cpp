#include "MyThread.h"

#include <QDebug>
MyThread::MyThread()
{

}

void MyThread::run() {
    qDebug() << "run thread:" << QThread::currentThreadId();
    while(1) {
        QString result = "running";
        emit resultReady(result);
        QThread::sleep(1);
    }
}

void MyThread::resultHandled() {
    qDebug() << "resultHandled thread:" << QThread::currentThreadId();
}
