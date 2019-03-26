#ifndef RTU_H__
#define RTU_H__
#include <sys/time.h>


#define UC			unsigned char
#define DEBUG
#define USI			unsigned short int

#define 	SUCCESS		0
#define 	ERRFORMAT 	1
#define 	ERRCRC		2
#define 	ERRLENGTH	3
#define 	ERRDTURECV	4

#define 	HEAD 		2
#define 	LENGTH 		1
#define 	DEST 		1
#define 	COMMAND		1
#define		CRC			2
//DSP协议的最大字节长度，目前为header + len + device + code + max(data) +crc = 57
#define 	MAXDSP 		1024	//DSP
#define		MAXDSPNUM	100		//DSP最大数量
#define		DATAREGNUM	12		//参数寄存器数目
#define		STATUSREGNUM	7	//状态寄存器数目
#define		DTURESP		11	//DTU响应信息长度


typedef enum Code { 
	C01 = 1, 
	C02, 
	C03,
	C04,
	C05,
	C06,
	C07,
	C08
} Code;

typedef struct {
	UC 		name;
	USI 	value;
}Reg;

typedef struct {
	UC 		start;
	UC 		num;
	/*     参数表  */
	Reg 	reg[12];
} Data;
//接受报文格式
typedef struct	{
	UC		*dsend;
	UC 		*drecv;
	UC		dsendlen;
	UC		drecvlen;
	USI 	head;			//帧头
	UC 		len;			//长度1 byte
	UC 		device;			//目的设备地址 1 byte
	Code 	code;			//命令编码 1 byte
    Data 	data;			//数据域 最长12
    USI 	status[7];		//状态表
    USI		param[12];		//状态表
    USI 	crc;			//crc校验码
}Dsp;


typedef struct {
	time_t	sec;
    UC      *dtumsg;            //发往dtu的信息块
    int     dtulen;             //dtu长度
    UC		*dtuinfo;			//发往dtu的信息缓存，每次扫描定时刷新
	UC		*dtuinfoend;		//指针指向dtuinfo结尾,dtu信息同步后需要重置
	UC		dturecv[11];		//dtu响应信息
	void 	*db;				//数据库   
    char    *logbuf;         	//日志缓存
    UC 		dspset[128];		//所有dsp集合列表   dsp[0]为dsp总数
    void    *dspbutton[128];
	int		dspfd;				//发往dsp的串口fd
	int		dtufd;				//发往dtu的串口fd
}Env;


typedef struct {
	UC 		data;
}Send;


/***********************************************************************/
/*			数据库调用封装API				*/
/***********************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

extern int testdtu();

extern Env *E;
extern Dsp *R;

/*******	以下部分为数据层业务调用API		****************/

extern void initEnv(int dspbandrate);

extern int reflushdspset(Env *E);          //从数据库查询dspset并填充到E->dspset
extern int gendspqueryone(UC deviceid,UC cmdtype,UC regstart, UC rnum, Dsp *R);
extern int gendspmodifyone(UC deviceid,UC cmdtype,UC rnum, Reg* regs, Dsp *R);
extern int sendquerycommandtoalldsp(UC cmdtype,UC regnum, Dsp *R, Env *E);//向所有dsp发生查询命令 cmdtype指定命令id, regnum指定查询寄存器数据
extern int scandsp(int num, Dsp *R, Env *E);
extern int send1and5comand(Dsp *R, Env *E);
extern int appenddtuinfo(Dsp *R, Env *E);

extern int doDsp(Dsp *R, Env *E);

extern void finish_R(Dsp *R);
extern void fill_sql(char *sql,Dsp *R); //生成动态sql语句，将结果保持在sql中

//SQLSERVER数据方法如无此API则返回NULL
extern void *DB_init();
extern void DB_close(void *db);
extern int setupdspset(Env *E);			//建立dspset数据表
extern int insertdspid(UC dspid,Env *E);  //数据插入dspid
extern int savetodb(Dsp *R,Env *E);   	//将信息写入数据库
extern int resetDB(Env *E);

/* crc 检验*/
extern USI crc_modbus(UC *,UC);

extern int updatedtu(Env *E);
extern int initlocaltime(Env *E);

extern int initdtuinfo(Env *E);
extern int updateLog(Env *E);
#ifdef __cplusplus
}
#endif
#endif
