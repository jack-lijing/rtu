#include "dspthread.h"

dspthread::dspthread()
{
}


//dsp_status 信号 0 成功  1 正在查询 2 失败   3 本轮查询开始 4 本轮查询结束
void dspthread::run()
{
    emit signal_dsp_status(128, 1);
    runlog("",2,"DSP QUERY START!");
    int i;
    int j = 0;
    for (i = 1; i <= E->ids[0]; i++)
    {
		int id = E->ids[i];
		E->dset[id].id = id;
        /****查询参数表************/
        emit signal_dsp_status(id, 1);
        if(doDsp(id,1) != 0 )   //如果doDsp失败则跳过此次查询
		{
            emit signal_dsp_status(id, -1);
            j++;
            continue;
		}
        Dsp *R = &E->dset[id].p;
        savetodb(R);
        sleep(1);

        /****查询状态表****/
       // gendspqueryone(id,0x05,0x07,0x00,R);
        if(doDsp(id,2) != 0 )   //如果doDsp失败则跳过此次查询
		{
            emit signal_dsp_status(id, -1);
            continue;
		}
        R = &E->dset[id].s;
        savetodb(R);
        emit signal_dsp_status(id, 0);
        sleep(7);
    }

    emit signal_dsp_status(128, 0);
    if(j == E->ids[0])
        emit signal_dsp_status(128,-1)	;
    exit();
}
