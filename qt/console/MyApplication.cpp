#include "MyApplication.h"
#include "MyObject.h"
#include "MyThread.h"

MyApplication::MyApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{

    MyObject *myobject = new MyObject(this);
    MyThread *mythread = new MyThread();
    QObject::connect(mythread, SIGNAL(destroyed(QObject*)), myobject, SLOT(destroyed(QObject*)));
    QObject::connect(mythread, SIGNAL(finished()), myobject, SLOT(finished()));
    QObject::connect(mythread, SIGNAL(objectNameChanged(QString)), myobject, SLOT(objectNameChanged(QString)));
    QObject::connect(mythread, SIGNAL(resultReady(QString)), myobject, SLOT(resultReady(QString)));
    QObject::connect(mythread, SIGNAL(started()), myobject, SLOT(started()));
    connect(myobject, SIGNAL(resultHandled()), mythread, SLOT(resultHandled()));
    mythread->start();
}

MyApplication::~MyApplication() {

}
