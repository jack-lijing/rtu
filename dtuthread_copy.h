#ifndef DTUTHREAD_H
#define DTUTHREAD_H

#include <QObject>
#include <QThread>
#include "rtu.h"
//#include "csapp.h"

class dtuthread : public QThread
{
public:
    dtuthread(Env *);
protected:
    void run();
private:
    Env *E;
    // status 0 成功  -1 失败   上报次数 1,2,3
signals:
    // status 0 正在查询  1 成功  2 失败
    void signal_dtu_status(int num,int status);
    void signal_dtu(int num,int status);
};

#endif // DTUTHREAD_H
