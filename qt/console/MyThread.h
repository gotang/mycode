#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>

class MyThread: public QThread
{
    Q_OBJECT
public:
    MyThread();
    virtual void run();
signals:
    void resultReady(const QString &s);
private slots:
    void resultHandled();
};

#endif // MYTHREAD_H
