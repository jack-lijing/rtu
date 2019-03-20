#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include   "rtu.h"

class mythread : public QThread
{
    Q_OBJECT
private:
    Env *E;
    Dsp *R;
    int num;
public:
    mythread(Env*,Dsp*,int);
protected:
    void run();
signals:
    void signal_scan(int,int);
};

#endif // MYTHREAD_H
