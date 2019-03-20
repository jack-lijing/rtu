#include	"csapp.h"
#include	"rtu.h"
#include	"uartconfig.h"



int check_Msg(Dsp *R)
{
	UC *m = &R->drecv[0];

    if (m[1] != 0xFF || m[0] != 0xFF || m[2] != R->drecvlen - 5)	//长度字段不能超过全长-5
	{
    //	printf("Error Message Format\n");
		return ERRFORMAT;
	}

	USI c = 0;
	//计算crc, crc_modbus(计算对象，计算长度), 结果在返回值中
	c = crc_modbus(&m[3],m[2]);
	//获取协议中携带的crc
    USI cr =(USI) *(m+3+m[2])<<8 | *(m+4+m[2]);
    //printf("crc cal = %04X crc in msg=%04X \n",c, cr);
	if (c != cr)
	{
		printf("Error Message CRC16\n");
        runlog("",1,"Error CRC %x:%x",c,cr);
		return ERRCRC;
	}

	return SUCCESS;
}

void finish_R(Dsp *R)
{
	free(R->drecv);
	free(R->dsend);
}

void fill_R(Dsp *R)
{
	R->head = R->drecv[0]<<8 | R->drecv[1];
	R->len = R->drecv[2];
	R->device = R->drecv[3];
	R->code = R->drecv[4];
	UC *p = &R->drecv[0];
	R->crc = *(p + 3 + R->len )<<8 | *(p + 4 + R->len);
}


void fill_Data(Dsp *R)
{
	R->data.num =R->drecv[6];
    UC i = 0;
	if (R->code == C01 || R->code == C05)
	{
		R->data.start =R->drecv[5]; 

        for (i = 0; i< R->data.num; i++ )
		{
			R->data.reg[ R->data.start + i ].name  = *(R->drecv+7+3*i); 
			R->data.reg[ R->data.start + i ].value = *(R->drecv+8+3*i)<<8 | *(R->drecv+8+3*i+1);
            if(R->code == C01)R->param[i]=R->data.reg[ R->data.start +i].value;
            if(R->code == C05)R->status[i]=R->data.reg[ R->data.start +i].value;

		}
	}
	if (R->code == C04 || R->code == C06)
	{
        for (i = 0; i< R->data.num; i++ )
		{
			int index = *(R->drecv+6+3*i);  //index为寄存器下标
			R->data.reg[index].name  = index; 
			R->data.reg[index].value = *(R->drecv+7+3*i)<<8 | *(R->drecv+7+3*i+1);
		}
	}
}

void printR(Dsp *R)
{
	printf(" %d 长度:%d 设备:%d 命令:%d",R->head,R->len,R->device, R->code);
	printf("  起始:%d 数量:%d \n",R->data.start,R->data.num);
    int i = 0;
    for (i = 0; i <R->data.num ; i++ )
	{
		int j = R->data.reg[i].name;
        printf("参数:%d 数值:d%05d x%04x\n",R->data.reg[j].name,R->data.reg[j].value, R->data.reg[j].value);
    }

    if(R->code == 0x01)
        for (i = 0; i < 12; i++)
            printf("%d ",R->param[i]);
    if(R->code == 0x05)
        for (i = 0; i < 7; i++)
            printf("%d ",R->status[i]);
    printf("\n");
}

int gendspqueryone(UC deviceid,UC cmdtype,UC regstart, UC rnum, Dsp *R)
{
//	unsigned char data_send_1[]={0xff,0xff,0x04,0x01,0x01,0x08,0x01,0x18,0x96};		// 命令编号01
    int len = 9;                    //此长度只适合顺序查询命令
	R->dsend = (UC *)Calloc(len,1);
	R->dsend[0] = 0xff;
	R->dsend[1] = 0xff;
	R->dsend[2] = 4 ;
	R->dsend[3] = deviceid;
	R->dsend[4] = cmdtype;
	R->dsend[5] = regstart;
	R->dsend[6] = rnum;

	//计算crc, crc_modbus(计算对象，计算长度), 结果在返回值中
	USI c = crc_modbus(&R->dsend[3],4);
	R->dsend[7] = c >> 8;
	R->dsend[8] = (c << 8) >> 8;

    R->drecvlen = len + rnum*3;
    R->drecv = (UC *)Calloc(R->drecvlen,1);
    //memcpy(R->drecv,R->dsend,7);
    //R->drecv[2] = rnum*3 + 4;
}

int gendspmodifyone(UC deviceid,UC cmdtype,UC rnum, Reg* regs, Dsp *R)
{
	int step = 3; 
	int len = 5 + 1 + rnum * step + 2;
	R->dsend = (UC *)Calloc(len,1);

	R->dsend[0] = 0xff;
	R->dsend[1] = 0xff;
	R->dsend[2] = len - 5 ;
	R->dsend[3] = deviceid;
	R->dsend[4] = cmdtype;

	//填充数据域
	R->dsend[6] = rnum;
	int i;
	for( i = 0 ; i < rnum ; i++)
	{
		R->dsend[6 + i*step] = regs[i].name;
		//注意USI为2 bytes,填充了2byte空间
		R->dsend[6 + i*step + 1] = regs[i].value >> 8;
		R->dsend[6 + i*step + 2] = (regs[i].value << 8) >> 8;
	}

	//计算crc, crc_modbus(计算对象，计算长度), 结果在返回值中
	USI c = crc_modbus(&R->dsend[3],R->dsend[2]);
	R->dsend[len-2] = c >> 8;
	R->dsend[len-1] = (c << 8) >> 8;

	/*********** 填充 *************/
	R->drecv = (UC *)Calloc(len,1);
	//响应消息和发送消息一样
	memcpy(R->drecv,R->dsend,len);

}

void fill_sql(char *sql, Dsp *R)
{
    char sqln[500] = {0};
    char sqlv[500] = {0};
    UC i = 0;
    if(R->code == C01 || R->code == C05)
        for ( i = 0; i< R->data.num; i++ )
        {
            char t1[4]={0};  //col name
            snprintf(t1,3,"%d",R->data.reg[ R->data.start+i ].name);
	//		printf("t1:%s,name:%d ", t1,R->data.reg[R->data.start + i].name);
            strcat(sqln, "r");
            strcat(sqln, t1);
            strcat(sqln, ",r");
            strcat(sqln, t1);
            strcat(sqln, "v,");

            char t2[6]={0}; //value
            snprintf(t2,5,"%d",R->data.reg[ R->data.start+i ].value);
            strcat(sqlv, t1);
            strcat(sqlv, ",");
            strcat(sqlv, t2);
            strcat(sqlv, ",");
        }
    if(R->code == C04 || R->code == C06)
        for ( i = 0; i< R->data.num; i++ )
        {
            int index = *(R->drecv+6+3*i);
            char t1[4]={0};
            snprintf(t1,2,"%d",R->data.reg[index].name);
            strcat(sqln, "r");
            strcat(sqln, t1);
            strcat(sqln, ",r");
            strcat(sqln, t1);
            strcat(sqln, "v,");

            char t2[6]={0};
            snprintf(t2,5,"%d",R->data.reg[index].value);
            strcat(sqlv, t1);
            strcat(sqlv, ",");
            strcat(sqlv, t2);
            strcat(sqlv, ",");
        }
    sqln[strlen(sqln)-1]='\0';
    sqlv[strlen(sqlv)-1]='\0';

    time_t sec;
    time(&sec);

    //printf("\nSQL SEC:%ld\n",sec);

    switch(R->code)
    {
        case C01:
            snprintf(sql,1000,"insert into data(device,%s,sec,date) values(%d,%s,%d,datetime('now')) ",sqln,R->device,sqlv,sec);
            break;
        case C05:
            snprintf(sql,1000,"insert into status(device,%s,sec,date) values(%d,%s,%d,datetime('now')) ",sqln,R->device,sqlv,sec);
            break;
        case C04:
            snprintf(sql,1000,"insert into data(device,%s,sec) values(%d,%s,%d,datetime('now')) ",sqln,R->device,sqlv,sec);
            break;
        case C06:
            snprintf(sql,1000,"insert into status(device,%s,sec) values(%d,%s,%d,datetime('now')) ",sqln,R->device,sqlv,sec);
            break;
    }

   // printf("Gen:%s\n",sql);
}

int doDsp(Dsp *R, Env *E)
{
	//send msg to dsp
	int dslen = R->dsend[2] + 5;
    int ch = Uart_send(E->dspfd, R->dsend, dslen,E->logbuf);
	
	//recv msg from dsp
	int drlen = 0;
    drlen = Uart_read(E->dspfd,R->drecv,R->drecvlen,E->logbuf);

    if(dslen == -1 || drlen != R->drecvlen)
    {
        printf("write:%d read:%d\n",dslen, drlen);
        return -1;
    }

    if(check_Msg(R))
    {
        runlog("",1,"Error Message Format");
        memset(R->drecv,0,R->drecvlen);
        return -1;
    }

    fill_R(R);
	fill_Data(R);
    //printR(R);
    tcflush(E->dspfd, TCIOFLUSH);
    return 0;
}

int appenddtuinfo(Dsp *R, Env *E)
{
	if(R->code == 0x01)
	{
		//copy 12 个寄存器--参数表
		memcpy(E->dtuinfoend, R->drecv + 7, 36);	
		E->dtuinfoend += 36;
	}else if(R->code == 0x05)
	{
		//copy 7个寄存器--状态表
		memcpy(E->dtuinfoend, R->drecv + 7, 21);	
		E->dtuinfoend += 21;
	}
}

int sendquerycommandtoalldsp(UC cmdtype,UC regnum, Dsp *R, Env *E)
{
    UC i;
    for ( i = 1; i <= E->dspset[0]; i++)
    {
        gendspqueryone(E->dspset[i],cmdtype,0x00,regnum,R);
        doDsp(R,E);
        savetodb(R,E);
		appenddtuinfo(R,E);
        memset(R,0,sizeof(Dsp));
    }
}

int send1and5comand(Dsp *R, Env *E)
{
    time_t sec;
    time(&sec);
	E->dtuinfo[4] = (sec >> 24) & 0xFF;
    E->dtuinfo[5] = (sec >> 16) & 0xFF;
	E->dtuinfo[6] = (sec >> 8) & 0xFF;
	E->dtuinfo[7] = sec & 0xFF;
	//注意机器差异大小端
	//memcpy(E->dtuinfo+4,&sec,4);
	printf("\nQUERY SEC:%d:%ld\n",sizeof(sec),sec);
	printf("\n:%02x:%02x:%02x:%02x\n",E->dtuinfo[4],E->dtuinfo[5],E->dtuinfo[6],E->dtuinfo[7]);
	
	E->dtuinfoend = E->dtuinfo + 8;

    UC i;
    for (i = 1; i <= E->dspset[0]; i++)
    {
        /****查询参数表************/
        gendspqueryone(E->dspset[i],0x01,0x00,0x0C,R);
        if(doDsp(R,E) != 0 )   //如果doDsp失败则跳过此次查询
            continue;
        savetodb(R,E);
        appenddtuinfo(R,E);
        memset(R,0,sizeof(Dsp));

        /****查询状态表****/
        gendspqueryone(E->dspset[i],0x05,0x00,0x07,R);
        if(doDsp(R,E) != 0)
            continue;
        savetodb(R,E);
        appenddtuinfo(R,E);
        memset(R,0,sizeof(Dsp));
    }
}

//初始化发往的dtu协议头
int initdtuinfo(Env *E)
{
    /***********创建dtu信息区块*****/
    UC len = E->dspset[0]*57 + 10; //57是19（寄存器）x3, 10是固定字段总和
    E->dtulen = len;
    E->dtuinfo = (UC*)Calloc(1,len);
    E->dtumsg = (UC*)Calloc(1,len);
    E->dtuinfo[0] = 0xFF;
    E->dtuinfo[1] = 0xFF;
    E->dtuinfo[2] = 0x05 + E->dspset[0]*19*3; //数据域长度
	E->dtuinfo[3] = 0x10;
    E->dtuinfoend = &E->dtuinfo[4];
    memcpy(E->dtumsg,E->dtuinfo,4);
}

/* 扫描 1 -- num 的DSP设备,刷新dspset表格,并将dsp id信息保存至 Env */
int scandsp(int num, Dsp *R, Env *E)
{
    setupdspset(E);     //重建dspset数据表
    UC i;
    for(i = 1; i <= num; i++)
	{
		gendspqueryone(i,0x01,0x00,0x01,R);
		doDsp(R,E);
		if(R->device != 0)
			insertdspid(R->device,E);  //数据插入dspid
		else 
			printf("%d is no exist\n",i);
		memset(R,0,sizeof(Dsp));
	}
	printf("End Scan Dsp\n");
	reflushdspset(E);
	Free(E->dtuinfo);
    Free(E->dtumsg);
	initdtuinfo(E);
}

int setlocalsec(Env *E)
{
	E->sec = 0;
	E->sec = (E->dturecv[5])<<24 | (E->dturecv[6]) <<16 | E->dturecv[7] << 8 | E->dturecv[8];

	/*系统BUG，暂时不设置*/
    stime(&E->sec);
    struct tm* local; //本地时间
    char buf[128]= {0};
    local = gmtime(&E->sec); //转为本地时间
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", local);
    runlog("",0,":DTU time %s",buf);
}

int initlocaltime(Env *E)
{
    UC s[6] = {0xFF,0xFF,0x01,0x10,0x8C,0xBE};
    int ch = Uart_send(E->dtufd, s, sizeof(s),E->logbuf);
	//recv second from dtu
	int drlen = 0;
    drlen = Uart_read(E->dtufd,E->dturecv,DTURESP,E->logbuf);
   if(drlen == DTURESP)dodtuMsg(E);
}

void initEnv(Env *E,char *logbuf,void *dbcon,int dspfd,int dtufd)
{
	E->logbuf = logbuf;
	E->dspfd = dspfd;
	E->db = dbcon;
	E->dtufd = dtufd;
	E->dtuinfo = NULL;
    reflushdspset(E);
	initlocaltime(E);
	initdtuinfo(E);
}

//对dtu 返回信息进行解析,正确就调整时钟并返回0, 错误返回1,上层redo
int dodtuMsg(Env *E){
    int r = 0;
   if(E->dturecv[0] != 0xff ||
      E->dturecv[1] !=0xFF ||
      E->dturecv[2] !=0x06 ||
      E->dturecv[3] !=0x10 ||
      E->dturecv[4] !=0x00 )
       r = -1;
	USI c = 0;
	c = crc_modbus(E->dturecv+5,4); //计算crc, crc_modbus(计算对象，计算长度), 结果在返回值中
    USI cr =(USI) *(E->dturecv+9)<<8 | *(E->dturecv+10);
    //printf("\nDTU CRC:%04x, msg:%02x%02x\n",c,E->dturecv[9],E->dturecv[10]);
	if (c != cr)
	{
        runlog("",2," receive %s; DTU CRC Error expect:%x get:%x ",E->dturecv,c,cr);
		return ERRCRC;
	}
    setlocalsec(E);
	return 0;
}



/* 发送dtumsg, 并将dtuinfoend指针复位,dtuinfo清零 */
int updatedtu(Env *E)
{
	/**添加crc校验**/
	USI c = 0;
    c = crc_modbus(E->dtumsg+3, 1 + 4 + E->dspset[0]*19*3); //计算crc, crc_modbus(计算对象，计算长度), 结果在返回值中
    memcpy(E->dtumsg+E->dtulen-2, &c, 2);
	
    //printf("Send DTUINFO len %02x",E->dtulen );
    int dslen = Uart_send(E->dtufd, E->dtumsg, E->dtulen,E->logbuf);
	//recv respond from dtu
    //UC recv[11] = {0};
    int drlen = Uart_read(E->dtufd,E->dturecv,11,E->logbuf);

    if (dslen == -1 || drlen != 11)
    {
        runlog("",2,"DTU Error send %d read %d",dslen,drlen);
        return -1;
    }
    if(dodtuMsg(E) !=0 )return -1;

    return 0;
}

int updateLog(Env *E)
{
    FILE *fd = fopen("/root/0309.log","r");
	char buff[255];
    while(fgets(buff,255,fd) != NULL)
	{
    	int i = Uart_send(E->dtufd,buff,strlen(buff),E->logbuf);
        printf("%d,send:%d",i,strlen(buff));
    //	sleep(3);
	}
    /*int drlen = Uart_read(E->dtufd,E->dturecv,11,E->logbuf);

    if (dslen == -1 || drlen != 11)
    {
        runlog("",2,"DTU Error send %d read %d",dslen,drlen);
        return -1;
    }
    if(dodtuMsg(E) !=0 )return -1;
*/
    return 0;
}

