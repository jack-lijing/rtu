#include "dtuthread.h"

dtuthread::dtuthread(Env *env)
{
    this->E = env;
}


//dsp_status 信号 0 成功  1 正在查询 2 失败   3 本轮查询开始 4 本轮查询结束
void dtuthread::run()
{

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
