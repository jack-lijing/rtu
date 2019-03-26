#include "zhuangtaibiao.h"
#include "ui_zhuangtaibiao.h"


zhuangtaibiao::zhuangtaibiao(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zhuangtaibiao)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rtu.db");
    if (!db.open())
        qDebug()<<"DB Open Fail"<<db.lastError();

    ui->setupUi(this);
    offset = 0;
    step = 10;
    ui->prev->setDisabled(true);
    connect(ui->fanhui,SIGNAL(clicked()), this, SLOT(hideZTB()));
}

void zhuangtaibiao::showtable(int )
{
    /*******Open Sqlite***********/

    QSqlQueryModel *model = new QSqlQueryModel(this);
    QString sql ;
    sql.sprintf("select id,device,r1v,r2v,r3v,r4v,r5v,r6v,date from status order by id desc limit %d offset %d",step,offset);
    qDebug()<<sql;
    model->setQuery( sql);

    model->setHeaderData(0,Qt::Horizontal,tr("id"));
    for(int i =1; i <= 7; i++ )
    {
        model->setHeaderData(i,Qt::Horizontal,i);
    }
    model->setHeaderData(8,Qt::Horizontal,tr("时间"));


    QTableView *view = ui->tableView;
    view->resizeColumnsToContents();
    view ->setModel(model);

    int width = 550/7;
    for(int i =0; i <=7; i++)
    {
        view->setColumnWidth(i, width);
    }
    //view->setColumnWidth(8,250);

}

void zhuangtaibiao::hideZTB()
{
    emit quitZTB();
}

zhuangtaibiao::~zhuangtaibiao()
{
    delete ui;
}

void zhuangtaibiao::on_next_clicked()
{
    ui->prev->setEnabled(true);
    offset += step;
    showtable(offset);
}


void zhuangtaibiao::on_prev_clicked()
{
    offset -= step;
    showtable(offset);
}

void zhuangtaibiao::on_first_clicked()
{
    ui->prev->setDisabled(true);
    offset = 0;
    showtable(offset);
}
