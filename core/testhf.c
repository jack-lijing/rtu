#include "uartconfig.h"
#include "csapp.h"
#include <unistd.h>
#include <termios.h>

int main()
{
	int fd = Uart_open("/dev/ttymxc1");
	Uart_485(fd);
	Uart_config(fd, B115200);

	char s[8]={0xff,0xff,0x02,0x05,0x00,0x07,0x5f,0x50};
	Uart_send(fd,s,8,NULL);
	
	char r[20] ;
	memset(r,0,20);
	Uart_read(fd,r,20,NULL);
	dumpBuffer(r,20);
}
