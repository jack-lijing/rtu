#include "dtuthread.h"

dtuthread::dtuthread(Env *e)
{
    this->E = e;
}

void dtuthread::run()
{
    //如果正在查询信息，则先挂起

    int i = 1;
    for(i=1; i<=3;)
        if(updatedtu(E) == -1)
        {
            emit signal_dtu_status(0,i);
            i++;
            emit signal_dtu_status(0,-1);
        }
        else
        {
            emit signal_dtu_status(0,0);
            break;
        }


}
