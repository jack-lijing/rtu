#include "uartconfig.h"


int Uart_open(const char* port)
{
	int fd_temp;
	int flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
	fd_temp = open(port, flags);
	if(fd_temp < 0)
		{
			perror("Can't open Serial Port");
			return(-1);
		}
    return fd_temp;
}

void Uart_close(int fd)
{
	close(fd);
}

int uart_config(int fd, int bandrate)
{
	struct termios tty;

	if(tcgetattr(fd, &tty) != 0)
		{
			perror("failed setup Serial");
			return(-1);
		}

	/*config bandrate*/
	cfsetispeed(&tty,bandrate);
	cfsetospeed(&tty,bandrate);

	/*config data bits = 8*/
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

	/*config parity = none*/
	tty.c_cflag &= ~PARENB;

	/*config stop bits = 1*/
	tty.c_cflag &= ~CSTOPB;

	/* config flow control = none*/
	tty.c_cflag &= ~CRTSCTS;

	/*config control mode*/
    tty.c_cflag |= CLOCAL;      //设备直连
    tty.c_cflag |= CREAD;       //可以接受字符

    /* config RAW data mode input and output*/
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	tty.c_oflag &= ~OPOST;   /*Output*/

	/*config wait time and minimal receive characters*/
    tty.c_cc[VTIME] = 100; /* wait time 10*(1/10)s */
    tty.c_cc[VMIN] = 0; /* minimal receive characters = 1 */

	/*data overrun processing*/
    tcflush(fd,TCIFLUSH);   //冲洗未决输出

	/* active above configuration*/
	if (tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		fprintf (stderr, "error %d from tcsetattr", errno);
		return -1;
	}
    return(1);
}

int Uart_send(int fd, char *buf_send, int buf_len, char *logbuf)
{
	int len;
	len = write(fd,buf_send,buf_len);
	if(len == buf_len)
	{
     //   printf("write successflly\n");
     //   dumpBuffer(buf_send,buf_len);
        if(logbuf != NULL)
        {
           i2ascii(logbuf,buf_send,buf_len);
           runlog("",0,":Send[%s]",logbuf);
        }
		return len;
	}
	else
	{
        runlog("",2,":Send Error expect %d but %d",buf_len,len);
		tcflush(fd,TCOFLUSH);
      //  printf("write error\n");
		return -1;
	}
}

int Uart_read(int fd, char *buf_read, int buf_len, char *logbuf)
{
	int len,fs_sel;
	fd_set fs_read;
	struct timeval time;

	FD_ZERO(&fs_read);
	FD_SET(fd,&fs_read);

    time.tv_sec = 10;    //设置超时定时 5s
	time.tv_usec = 0;

	fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    //fs_sel 0 没有准备好， 1 准备好一个，2 准备好2个，……， -1 出错了
 // runlog("",3,":select return:%d",fs_sel);
    if(fs_sel >=0)
	{
	   len = read(fd,buf_read,buf_len);
       if(len == buf_len)
       {
     //      dumpBuffer(buf_read, len);
           if(logbuf != NULL)
           {
               i2ascii(logbuf,buf_read,len);
               runlog("",0,":Read[%s]",logbuf);
           }
           return len;
       }
       else if(len >= 0)
       {
           dumpBuffer(buf_read, len);
           if(logbuf != NULL)
           {
               i2ascii(logbuf,buf_read,len);
               runlog("",2,":Read[%s] errno:%d,%s",logbuf,strerror(errno));
           }
           return len;
       }
       else
       {
           if(logbuf != NULL)
                runlog("",2,":expect[%d]get[%d],errno:%d,%s",buf_len,len,errno,strerror(errno));
           return -1;
       }

	}
	else
	{
        if(logbuf != NULL)
            runlog("",2,"select function error code %d!errno:%d,%s ",fs_sel,errno,strerror(errno));
	    return -1;
	}
}

int uart_485(int fd)
{
	struct serial_rs485 rs485conf;

	/* Enable RS485 mode: */
	rs485conf.flags |= SER_RS485_ENABLED;

	/* Set logical level for RTS pin equal to 0 when sending: */
	rs485conf.flags |= SER_RS485_RTS_ON_SEND;
	/* or, set logical level for RTS pin equal to 1 when sending: */
	//rs485conf.flags &= ~(SER_RS485_RTS_ON_SEND);

	/* Set logical level for RTS pin equal to 0 after sending: */
	//rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
	/* or, set logical level for RTS pin equal to 1 after sending: */
	rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);

	/* Set rts delay before send, if needed:(in microseconds) */
	rs485conf.delay_rts_before_send = 100;

	/* Set rts delay after send, if needed: (in microseconds) */
	rs485conf.delay_rts_after_send = 100;

	/* Set this flag if you want to receive data even whilst sending data */
	//rs485conf.flags |= SER_RS485_RX_DURING_TX;

	if (ioctl(fd, TIOCSRS485, &rs485conf) < 0) {
		printf("Error: TIOCSRS485 ioctl not supported.\n");
		return -1;
	}
	else
		return 1;
}

void Uart_config(int fd, int bandrate)
{
	if(uart_config(fd,bandrate) == -1)
	{
		printf("Uart bandrate setup error!");
		exit(-1);
	}
}

void Uart_485(int fd)
{
	if(uart_485(fd) < 0)
	{
		printf("failed enable RS485 mode\n");
		exit(-1);
	}
	else
		printf("RS485 mode enabled\n");
}



