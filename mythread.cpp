#include "mythread.h"
#include <unistd.h>


mythread::mythread(Env *env, Dsp* dsp,int n)
{
    E=env;
    R=dsp;
    num = n;
}

void mythread::run()
{
    setupdspset(E);     //重建dspset数据表
    UC i;
    emit signal_scan(128,1);
    for(i = 1; i <= num; i++)
    {
        gendspqueryone(i,0x01,0x00,0x01,R);
        if(doDsp(R,E) == 0 && R->device == i)
        {
            insertdspid(R->device,E);  //数据插入dspid
            emit signal_scan(i,0);
        }
        else
        {
            emit signal_scan(i,-1);
        }
        memset(R,0,sizeof(Dsp));
        sleep(5);
    }

    emit signal_scan(128,2);
    printf("End Scan Dsp\n");
    reflushdspset(E);
    //Free(E->dtuinfo);
    initdtuinfo(E);
}
