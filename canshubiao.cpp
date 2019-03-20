#include "canshubiao.h"
#include "ui_canshubiao.h"


canshubiao::canshubiao(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::canshubiao)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rtu.db");
    if (!db.open())
        qDebug()<<"DB Open Fail"<<db.lastError();

    ui->setupUi(this);
    offset = 0;
    step = 10;
    showtable(offset);
    ui->prev->setDisabled(true);
    connect(ui->fanhui,SIGNAL(clicked()), this, SLOT(hideCSB()));
}

void canshubiao::showtable(int )
{
    /*******Open Sqlite***********/

    QSqlQueryModel *model = new QSqlQueryModel(this);
    QString sql ;
    sql.sprintf("select id,device,r1v,r2v,r3v,r4v,r5v,r6v,r7v,r8v,r9v,r10v,r11v,date from data order by id desc limit %d offset %d",step,offset);
    qDebug()<<sql;
    model->setQuery( sql);

    model->setHeaderData(0,Qt::Horizontal,tr("id"));
    for(int i =1; i <= 12; i++ )
    {
        model->setHeaderData(i,Qt::Horizontal,i);
    }
    model->setHeaderData(13,Qt::Horizontal,tr("时间"));


    QTableView *view = ui->tableView;
    view->resizeColumnsToContents();
    view ->setModel(model);

    int width = 550/12;
    for(int i =0; i <=12; i++)
    {
        view->setColumnWidth(i, width);
    }
    view->setColumnWidth(13,250);

}

void canshubiao::hideCSB()
{
    emit quitCSB();
}

canshubiao::~canshubiao()
{
    delete ui;
}

void canshubiao::on_next_clicked()
{
    ui->prev->setEnabled(true);
    offset += step;
    showtable(offset);
}


void canshubiao::on_prev_clicked()
{
    offset -= step;
    showtable(offset);
}

void canshubiao::on_first_clicked()
{
    ui->prev->setDisabled(true);
    offset = 0;
    showtable(offset);
}
