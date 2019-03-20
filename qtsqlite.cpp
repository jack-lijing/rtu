#include    <QCoreApplication>
#include    <QSql>
#include    <QSqlError>
#include    <QSqlDatabase>
#include    <QSqlError>
#include    <QSqlQuery>
#include    <QString>
#include    <QFile>
#include    <QDebug>

#include 	"rtu.h"
#include 	"csapp.h"


int savetodb(Dsp *R)
{
    char *sql = (char *)calloc(1000,1);
    fill_sql(sql,R);
    QSqlQuery query;
    query.prepare(sql);
    if(!query.exec()){
        qDebug()<<query.lastError();
    }
    free(sql);
}

void *DB_init()
{
    /*
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rtu.db");

    if (!db.open())
    {
        qDebug()<<db.lastError();
        exit(1);
    }
    */
 //       return db.lastError();
  //  return QSqlError();
}

void DB_close(void *con)
{

}

int DB_exec(void *con ,const char *SQL, 
			int (*callback)(void*,int,char**,char**), 
			void *data)
{

    qDebug()<<SQL;
    QSqlQuery sql_query;
    sql_query.prepare(SQL);

    if(!sql_query.exec()){
        qDebug()<<sql_query.lastError();
	}else{
        qDebug()<<"Record created successfully";
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





int  savetofile(Dsp *R, int file)
{
	
}
