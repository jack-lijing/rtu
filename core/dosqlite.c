#include 	<string.h>
#include	"sqlite3.h"
#include 	"rtu.h"
#include 	"csapp.h"


void *DB_init()
{
	sqlite3 *db = NULL;
	int rc;
	rc = sqlite3_open("rtu.db", &db);
	if(rc){
		fprintf(stderr,"Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else{
		fprintf(stderr, "Opened database successfully\n");
	}
	return db;
}

void DB_close(void *con)
{
	sqlite3_close((sqlite3 *)con);
}

int DB_exec(void *con ,const char *SQL, 
			int (*callback)(void*,int,char**,char**), 
			void *data)
{
	int rc;
	char *errmsg = 0;
//	printf("%s\n",SQL);
	rc = sqlite3_exec((sqlite3 *)con, SQL, callback, data, &errmsg);
	if(rc != SQLITE_OK){
		fprintf(stderr,"SQL error: %s\n",errmsg);
		sqlite3_free(errmsg);
	}else{
        //fprintf(stdout,"Record created successfully\n");
	}
	return	SUCCESS;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i = 0; i < argc; i++){
		printf("%s = %s\n",azColName[i],argv[i]?argv[i]:"NULL");
	}
	printf("\n");
	return 0;
}


int savetodb(Dsp *R)
{
    char *sql = (char *)calloc(1024,1);
	fill_sql(sql,R);
	sqlite3 *con = (sqlite3 *)E->db;
	DB_exec(con,sql,callback, NULL);
    free(sql);
    return 0;
}


int setupdspset(Env *E)
{
	DB_exec((sqlite3 *)E->db,"drop table dspset;",NULL,NULL);
	printf("Create dspset table\n");
	DB_exec((sqlite3 *)E->db,"create table dspset(did int primary key not null);",NULL,NULL);
    return 0;
}

int insertdspid(UC dspid,Env *E)
{
	char *sql = Calloc(1,100);
    snprintf(sql,100,"insert into dspset(did) values(%d);",dspid);
	DB_exec((sqlite3 *)E->db,sql,NULL,NULL);
	printf("insert dsp id:%s\n",sql);
	free(sql);
    return 0;
}

/* 
 * 1 从数据库中取出 dsp id集合,保存在Env中 ,Env->ids[0]为dsp总数,dspset[1]为dsp地址1等
 * */
int reflushdspset(Env *E)
{
	char *sql = "select did from dspset;";
	char *errmsg;
	char** pResult;
	int nRow;
	int nCol;
	int nResult = sqlite3_get_table((sqlite3 *)E->db,sql,&pResult,&nRow,&nCol,&errmsg);
	if(nResult != SQLITE_OK)
	{
		printf("%s", errmsg);
		sqlite3_free(errmsg);
		return nResult;
	}
	E->ids[0] = (UC)nRow;
	printf("ids total:%d\n",E->ids[0]);
    int i;
    for(i = 1 ; i <= nRow; i++)
	{
		E->ids[i]=(UC)atoi(pResult[i]);
		printf("dspid in E:%d\n",E->ids[i]);
	}
	sqlite3_free_table(pResult);
	/*
	if(E->dtuinfo != NULL)
	{
		free(E->dtuinfo);
	}
	*/
	return 0;
}

/*清空并重建 data, status表*/
int resetDB(Env *E)
{
	DB_exec((sqlite3 *)E->db,"drop table data;",NULL,NULL); 
	DB_exec((sqlite3 *)E->db,"create table data(id integer primary key autoincrement, device int not null,r0 int, r0v int,r1 int, r1v int, r2 int, r2v int,r3 int,r3v int, r4 int,r4v int,r5 int, r5v int, r6 int,r6v int,r7 int,r7v int,r8 int,r8v int,r9 int,r9v int,r10 int,r10v int,r11 int, r11v int, sec int not null, date text not null); ", NULL, NULL);
	DB_exec((sqlite3 *)E->db,"drop table status;",NULL,NULL); 
	DB_exec((sqlite3 *)E->db,"create table status(id integer PRIMARY key autoincrement,device int NOT NULL,r0 int,r0v int,r1 int, r1v int, r2 int, r2v int,r3 int,r3v int, r4 int,r4v int,r5 int, r5v int, r6 int,r6v int , sec int not null,date text not null);",NULL,NULL);
    DB_exec((sqlite3 *)E->db,"drop table dspset;",NULL,NULL);
    DB_exec((sqlite3 *)E->db,"create table dspset(did integer PRIMARY key not null);",NULL,NULL);

    return 0;
}
