#include "dspthread.h"

dspthread::dspthread(Env *env, Dsp *dsp)
{
    this->E = env;
    this->R = dsp;
}


//dsp_status 信号 0 成功  1 正在查询 2 失败   3 本轮查询开始 4 本轮查询结束
void dspthread::run()
{
    //状态更新时锁定dtumsg，不允许做上报/
    time_t sec;
    time(&sec);
    E->dtuinfo[0] = 0xFF;
    E->dtuinfo[1] = 0xFF;
    E->dtuinfo[2] = 0x05 + E->dspset[0]*19*3; //数据域长度
    E->dtuinfo[3] = 0x10;
	E->dtuinfo[4] = (sec >> 24) & 0xFF;
	E->dtuinfo[5] = (sec >> 16) & 0xFF;
	E->dtuinfo[6] = (sec >> 8) & 0xFF;
	E->dtuinfo[7] = sec & 0xFF;
	
	E->dtuinfoend = E->dtuinfo + 8;
    emit signal_dsp_status(128, 1);
    runlog("",2,"DSP QUERY START!");
    UC i;
    int j = 0;
    for (i = 1; i <= E->dspset[0]; i++)
    {
        /****查询参数表************/
        memset(R,0,sizeof(Dsp));
        emit signal_dsp_status(E->dspset[i], 1);
        //gendspqueryone(E->dspset[i],0x01,0x0C,0x00,R);      //第二版协议
        gendspqueryone(E->dspset[i],0x01,0x00,0x0C,R);    //第一版协议
        if(doDsp(R,E) != 0 )   //如果doDsp失败则跳过此次查询
		{
            emit signal_dsp_status(E->dspset[i], -1);
            //若dsp无响应，则将本段数据填充为0
            UC k = 0;
            for( k = 0; k < 12; k++);
            {
                *(E->dtuinfoend + k*3) = k;
            }
            *(E->dtuinfoend)= 0x00;
            *(E->dtuinfoend+1) = 0x00;
            *(E->dtuinfoend+2) = E->dspset[i];
            E->dtuinfoend += 36;        //dsp的参数表 12*3

            for( k = 0; k <7; k++)
                *(E->dtuinfoend + k*3) = k;
            E->dtuinfoend +=21;
            //memset(R,0,sizeof(Dsp));
            j++;
            continue;
		}
        savetodb(R,E);
        appenddtuinfo(R,E);
        //memset(R,0,sizeof(Dsp));
        sleep(1);

        /****查询状态表****/
       // gendspqueryone(E->dspset[i],0x05,0x07,0x00,R);
        gendspqueryone(E->dspset[i],0x05,0x00,0x07,R);
        if(doDsp(R,E) != 0 )   //如果doDsp失败则跳过此次查询
		{
            emit signal_dsp_status(E->dspset[i], -1);
            UC k;
            for( k = 0; k <7; k++)
                *(E->dtuinfoend + k*3) = k;
            E->dtuinfoend +=21;
            //memset(R,0,sizeof(Dsp));
            continue;
		}
        savetodb(R,E);
        appenddtuinfo(R,E);
        emit signal_dsp_status(E->dspset[i], 0);
        //memset(R,0,sizeof(Dsp));
        sleep(7);
    }

    memcpy(E->dtumsg,E->dtuinfo,E->dtulen);
    //memset(E->dtuinfo, 0 , E->dtulen);
    E->dtuinfoend = E->dtuinfo+4;
    emit signal_dsp_status(128, 0);
    if(j == E->dspset[0])
        emit signal_dsp_status(128,-1);
    exit();
}
