#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QObject>

class MyObject : public QObject
{
    Q_OBJECT
public:
    MyObject(QObject *parent = 0);
signals:
    void resultHandled();
private slots:
    void resultReady(const QString &s);
    void started();
    void finished();
    void objectNameChanged(QString s);
    void destroyed(QObject *o);
};

#endif // MYOBJECT_H
