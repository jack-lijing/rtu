#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include   "rtu.h"

class mythread : public QThread
{
    Q_OBJECT
public:
    mythread();
protected:
    void run();
signals:
    void signal_scan(int,int);
};

#endif // MYTHREAD_H
