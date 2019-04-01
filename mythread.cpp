#include "mythread.h"
#include <unistd.h>

mythread::mythread()
{
}

void mythread::run()
{
    setupdspset(E);     //重建dspset数据表
    UC i;
    emit signal_scan(128,1);
    for(i = 1; i <= E->ini.devices; i++)
    {
        if(doDsp(i,1) == 0 && E->dset[i].id== i)
        {
            insertdspid(E->dset[i].id,E);  //数据插入dspid
            emit signal_scan(i,0);
        }
        else
        {
            emit signal_scan(i,-1);
        }
        sleep(5);
    }

    emit signal_scan(128,2);
    printf("End Scan Dsp\n");
    reflushdspset(E);
    //Free(E->dtuinfo);
    initdtuinfo(E);
}
