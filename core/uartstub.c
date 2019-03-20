#include "uartconfig.h"

int Uart_open(const char* port)
{
	return 1;
}

void Uart_close(int fd)
{
}

void Uart_config(int fd, int bandrate)
{
}

void Uart_485(int fd)
{
}

int Uart_send(int fd, char *buf_send, int buf_len)
{
	return 1;
}

int Uart_read(int fd, char *b, int buf_len)
{
	//char r[] = {0xff,0xff,0x07,0x01,0x01,0x08,0x01,0x08,0x88,0x88,0x88,0x0D};
	char r[] = {0xff,0xff,0x07,0x01,0x01,0x08,0x01,0x08,0x88,0x88,0x88,0x0D};
	memcpy(b, r, buf_len);
	return buf_len;

}


/*
	unsigned char data_send_4[]={0xff,0xff,0x09,0x01,0x04,0x02,0x08,0x88,0x88,0x09,0x99,0x99,0x9C,0x95};				// 命令编号04
	unsigned char data_send_5[]={0xff,0xff,0x04,0x01,0x05,0x02,0x02,0x78,0x91};		// 命令编号05
	unsigned char data_send_6[]={0xff,0xff,0x09,0x01,0x06,0x02,0x04,0x00,0x44,0x06,0x00,0x66,0xD5,0xD1};				// 命令编号06
	unsigned char* data_send_buf[] = { data_send_1, data_send_4, data_send_5, data_send_6};
*/
