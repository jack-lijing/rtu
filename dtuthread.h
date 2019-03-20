#ifndef DTUTHREAD_H
#define DTUTHREAD_H

#include <QObject>
#include <QThread>
#include "rtu.h"

class dtuthread : public QThread
{
    Q_OBJECT
public:
    dtuthread(Env *E);
protected:
    void run();
private:
    Env *E;
signals:
    // status 0 正在查询  1 成功  2 失败
    void signal_dtu_status(int num,int status);

};

#endif // DTUTHREAD_H
