#include	"csapp.h"
#include	"uartconfig.h"
#include	"rtu.h"

/******************Ö÷º¯Êý*************************/
int main(int argc, char *argv[])
{
    int dtufd =Uart_open("/dev/ttymxc2");
    Uart_485(dtufd);
    Uart_config(dtufd, B115200);

    int logbuf = (char *)Calloc(5120,1);

	char dturecv[11];
	while(1)
	{
		int drlen = Uart_read(dtufd,dturecv,DTURESP,logbuf);
//    int len = MAXDSP*57 + 10;
		int len = 10;
		UC s[len];
		memset(s,0x41,len);
		s[2]=len-3;
		int ch = Uart_send(dtufd, s, len,logbuf);
	}
}

