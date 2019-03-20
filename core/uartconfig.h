#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */

#define TIOCGRS485      0x542E
#define TIOCSRS485      0x542F

/*
#define NDST485			1 		//no data send to 485
#define LDST485			2		//less data send to 485
#define NDRF485			3		//no data receive from 485
*/
#ifdef __cplusplus
extern "C" {
#endif
int Uart_open(const char* port);
void Uart_close(int fd);
void Uart_config(int fd, int bandrate);
int Uart_send(int fd, char *buf_send, int buf_len, char *log);
int Uart_read(int fd, char *buf_read, int buf_len, char *log);
void Uart_485(int fd);
#ifdef __cplusplus
}
#endif
