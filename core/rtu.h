#ifndef RTU_H__
#define RTU_H__
#include <sys/time.h>
#include <pthread.h>


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
#define 	MAXDSP 			20				//DSP最大数量
#define		REGNUM			19				//DSP总寄存器数目
#define		MAXREG			12				//最多类寄存器数目
#define		STATUSREGNUM	7				//状态寄存器数目
#define		DTURESP			11					//DTU响应信息长度
#define		MDSPBUFFER	(MAXREG*3+9)		//DSP消息体最长长度

#define		ARM		1				//控制版本， 缺省为linux版本

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
    UC		dsend[MDSPBUFFER];		//发送请求
    UC		dsendlen;					//发送字长
    UC 		drecv[MDSPBUFFER];					//响应缓存
    UC		drecvlen;					//响应字长
    USI 	head;						//帧头
    UC 		len;						//长度1 byte
	UC 		device;			//目的设备地址 1 byte
	Code 	code;			//命令编码 1 byte
    Data 	data;			//数据域 最长12
    USI 	crc;			//crc校验码
}Dsp;

typedef struct {
    UC 		id;
    UC		reg[REGNUM*3];		//参数id+参数值+状态id+状态值集
    USI		param[12];		//参数表
    USI 	status[7];		//状态表
	Dsp		p;				//参数消息缓存
	Dsp		s;				//状态消息缓存
}DspSet;

typedef struct {
    int		dsprate;		//默认B115200
    int		dspwait;		//dsp串口等待时长，暂时不实现
    int		dturate;		//默认B115200
    int		devices;		 //DSP最大编号，用于扫描，默认为20
    int		autodsp;		//无人值守模式,默认开启1
    int		dsptimer;		//dsp扫描频率，分钟计算，默认3分钟
    int		dtutimer;		//dtu上报频率，小时计算，默认1小时

}Conf;
typedef struct {
	time_t	sec;
    int     dtulen;             //dtu长度
    UC		*dtuinfo;			//发往dtu的信息缓存，每次扫描定时刷新
    UC		dturecv[DTURESP];		//dtu响应信息
	void 	*db;				//数据库   
    char    *logbuf;         	//日志缓存
    UC 		ids[MAXDSP];		//存储所有dsp集合列表   dsp[0]为dsp总数
    DspSet	dset[MAXDSP];		//DSP内存表，初始化分配了所有id的内存
								//每次查询更新相应item
								//dset[0]接受单独指令信息
	int		dspfd;				//发往dsp的串口fd
	int		dtufd;				//发往dtu的串口fd
    Conf	ini;
}Env;

/***********************************************************************/
/*			数据库调用封装API				*/
/***********************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

extern int testdtu();

extern Env *E;

extern pthread_mutex_t dsplock;
extern pthread_mutex_t dtulock;

/*******	以下部分为数据层业务调用API		****************/

extern void initEnv();

extern int reflushdspset(Env *E);          //从数据库查询dspset并填充到E->dspset
extern int gendsporder(UC deviceid,UC cmdtype,UC regstart, UC rnum, Dsp *R);
extern int gendspmodifyone(UC deviceid,UC cmdtype,UC rnum, Reg* regs, Dsp *R);
extern int scandsp(int num);

/* type 1 : 参数表 2： 状态表*/
extern int doDsp(int dspid, int type);

extern void fill_sql(char *sql,Dsp *R); //生成动态sql语句，将结果保持在sql中

//SQLSERVER数据方法如无此API则返回NULL
extern void *DB_init();
extern void DB_close(void *db);
extern int setupdspset(Env *E);			//建立dspset数据表
extern int insertdspid(UC dspid,Env *E);  //数据插入dspid
extern int savetodb(Dsp *R);   	//将信息写入数据库
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
