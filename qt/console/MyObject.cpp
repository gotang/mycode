#include "MyThread.h"
#include "MyObject.h"

#include <QThread>
#include <QDebug>

MyObject::MyObject(QObject *parent):
    QObject(parent)
{

}

void MyObject::resultReady(const QString &s) {
    qDebug() << "resultReady:" << s;
    qDebug() << "resultReady thread " << QThread::currentThreadId();
    emit resultHandled();
}

void MyObject::started() {
    qDebug() << "started thread " << QThread::currentThreadId();
}

void MyObject::finished() {
    qDebug() << "finished thread " << QThread::currentThreadId();
}

void MyObject::objectNameChanged(QString s) {
    qDebug() << "objectNameChanged:" << s;
    qDebug() << "objectNameChanged thread " << QThread::currentThreadId();
}

void MyObject::destroyed(QObject *o) {
    qDebug() << "objectNameChanged:" << o->objectName();
    qDebug() << "destroyed thread " << QThread::currentThreadId();
}


