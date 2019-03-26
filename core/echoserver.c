#include "csapp.h"

void echo(int conn);
void readnb(int conn);

int main(int argc, char *argv[])
{/*
	if (argc != 2)
	{
		printf("usage: %s <port>\n",argv[0]);
	}
*/	
	int port=5901, listenfd;
//	port = atoi(argv[1]);
	int listendfd;
	listenfd = Open_listenfd(port);
	struct sockaddr_in caddr;
	int addrlen = sizeof(caddr);
	int conn;
	while(1)
	{
		conn = Accept(listenfd, (struct sockaddr *)&caddr, (int *)&addrlen);
		printf("Client Connect: %s:%d\n", inet_ntoa(caddr.sin_addr), caddr.sin_port);
		//char *sbuf = "123456789ab";
		//Rio_writen(conn, sbuf, n);
//		echo(conn);
		readnb(conn);
	//	Close(conn);
	}
	exit(0);
}

void readnb(int conn)
{
	rio_t rio;
	Rio_readinitb(&rio, conn);
	char usrbuf[2046];
	char len[3];
	int n =0;
	n = (int)Rio_readnb(&rio,len,3);
	if(len[2] > 0)
		n=(int)Rio_readnb(&rio,usrbuf,2046);
	
	printf("server receive %d bytes\n",n);
	dumpBuffer(usrbuf,n);
}

void echo(int conn)
{
	rio_t rio;
	Rio_readinitb(&rio, conn);
	char usrbuf[MAXLINE];
	char buf[MAXLINE];
	char sbuf[11] = {0xff,0xff,0x06,0x10,0x00,0x5c,0x90,0xa4,0x0d,0x9e,0xe8};
	int n;
	while((n = (int)Rio_readnb(&rio, usrbuf, 6)) != 0)
	{
		printf("server receive %d bytes\n",n);
		dumpBuffer(usrbuf,n);
	//	i2ascii(buf,usrbuf,n);
	//	printf("%s\n",usrbuf);
		Rio_writen(conn, sbuf, 11);
	}
} 
