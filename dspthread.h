#ifndef DSPTHREAD_H
#define DSPTHREAD_H

#include <QObject>
#include <QThread>
#include "rtu.h"
#include "csapp.h"

class dspthread : public QThread
{
    Q_OBJECT
public:
    dspthread();
    ~dspthread(){
    }
protected:
    void run();
private:

signals:
    // status 0 正在查询  1 成功  2 失败
    void signal_dsp_status(int num,int status);

};

#endif // DSPTHREAD_H
