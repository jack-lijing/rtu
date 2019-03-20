#include	"csapp.h"
#include	"uartconfig.h"
#include	"rtu.h"

/******************主函数*************************/
int main(int argc, char *argv[])
{
	int dspfd = 0;
	
	char *logbuf = (char *)Calloc(1024,1);
//	UC* data_send_buf[] = { data_send };
	//gendspmodifyone(UC deviceid,UC cmdtype,UC rnum, Reg* regs, R);

	/***** Open DSP 485 serial *****/
	dspfd = Uart_open("/dev/ttymxc1");
	Uart_485(dspfd);
	Uart_config(dspfd,B115200);
	/***** Open DTU serial *****/
	int dtufd = Uart_open("/dev/ttymxc2");
	Uart_485(dtufd);
	Uart_config(dtufd,B115200);

	/******* 连接数据库 ************/
	void *dbcon = DB_init();

	/********************初始化存储结构****/
	Dsp *R = (Dsp *)Calloc(1,sizeof(Dsp));

	Env *E = (Env *)Calloc(1,sizeof(Env));
	initEnv(E,logbuf,dbcon,dspfd,dtufd);
	//resetDB(E);

	scandsp(6, R, E);
   lockf(E->dspfd, F_LOCK, 0L);
   send1and5comand(R, E);
   lockf(E->dspfd, F_ULOCK, 0L);
	
   lockf(E->dspfd, F_LOCK, 0L);
//   updatedtu(E);
   lockf(E->dspfd, F_ULOCK, 0L);

	DB_close(dbcon);
	Uart_close(dspfd);
	Uart_close(dtufd);
	/*
	Free(logbuf);
	Free(E->dtuinfo);
	Free(E);
	Free(R);
	*/
}

