#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QStandardPaths>
#include <QtConcurrent>
#include <functional>
using namespace QtConcurrent;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /***************读取配置文件************/
    first = 1;
    set = new QSettings("/opt/rtu/config.ini", QSettings::IniFormat);
    devices = set->value("devices").toInt();
    int autodsp = set->value("autodsp").toInt();
    int b = set->value("rate").toInt();
    switch(b){
    case 9600:
        bandrate = B9600;
        break;
    case 57600:
        bandrate = B57600;
        break;
    case 115200:
        bandrate = B115200;
        break;
    case 460800:
        bandrate = B460800;
        break;
    }

    initEnv(bandrate);

    runlog("",2,"RTU START:Devices %d DSPTimer %d minutes DTUTimer %d hours bandrate %d",
           devices,set->value("dsptimer").toInt(),set->value("dtutimer").toInt(),b);
    ui->setupUi(this);
    ui->tz->setDisabled(true);
    /************ 槽响应 ******************/
    connect(ui->tuichu,SIGNAL(clicked()), this, SLOT(softExitRestart()));
    connect(ui->sz,SIGNAL(clicked()),this,SLOT(showSZ()));
    connect(ui->csb,SIGNAL(clicked()),this,SLOT(showCSB()));
    connect(ui->ztb,SIGNAL(clicked()),this,SLOT(showZTB()));
    connect(ui->scan,SIGNAL(clicked()),this,SLOT(on_scan_clicked()));
    connect(ui->uploadlog,SIGNAL(clicked()),this,SLOT(on_uploadlog_clicked()));

    connect(&sz,SIGNAL(quitSZ()), this, SLOT(hideSZ()));
    connect(&csb,SIGNAL(quitCSB()), this, SLOT(hideCSB()));
    connect(&ztb,SIGNAL(quitZTB()), this, SLOT(hideZTB()));

    connect(ui->yx,SIGNAL(clicked()), this, SLOT(startRTU()));
    connect(ui->tz,SIGNAL(clicked()), this, SLOT(stopRTU()));



    dspthd = new dspthread(E,R);
    connect(dspthd,SIGNAL(signal_dsp_status(int,int)), this, SLOT(displayDSP_slot(int,int)));

    dtuthd = new dtuthread(E);
    connect(dtuthd,SIGNAL(signal_dtu_status(int,int)), this, SLOT(displayDTU_slot(int,int)));

    scanthd = new mythread(E,R,devices);
    connect(scanthd,SIGNAL(signal_scan(int,int)), this, SLOT(scanDSP_slot(int,int)));

    /******显示时钟******/
    clockTimer = new QTimer(this);
    connect(clockTimer,SIGNAL(timeout()), this, SLOT(refreshTime_slot()));
    clockTimer->start(1000);

    /****如果无人值守模式开启，启动定时查询机制***/
    if(autodsp == 1)startRTU();
    //on_scan_clicked();
   // updateLog(E);
    testdtu();
}


void MainWindow::startRTU()
{
    runlog("",2,"Start QUERY DSP!");
    set = new QSettings("/opt/rtu/config.ini", QSettings::IniFormat);
    int dspintval = set->value("dsptimer").Int*1000*60;
    int dtuintval = set->value("updatetimer").Int*1000*3600;
    /***********启动DSP查询线程 * (定时机制:发送信号) ************/

    queryDSP();     //不等待计数器，先执行第一次
    dspTimer = new QTimer(this);
    connect(dspTimer,SIGNAL(timeout()), this, SLOT(queryDSP()));
    dspTimer->start(dspintval);

    /***********启动DTU上报****************/
    dtuTimer =  new QTimer(this);
    connect(dtuTimer,SIGNAL(timeout()), this, SLOT(updateDTU()));
    dtuTimer->start(dtuintval);

    /*****冻结按钮********/
    ui->yx->setDisabled(true);
    ui->tz->setEnabled(true);
    ui->sz->setDisabled(true);
    ui->scan->setDisabled(true);
    ui->dspprocess->setText("查询即将开始");
}

void MainWindow::stopRTU()
{
     runlog("",2,"STOP QUERY DSP!");
    if(dspTimer->isActive())
        dspTimer->stop();
    if(dtuTimer->isActive())
        dtuTimer->stop();

    ui->tz->setDisabled(true);
    ui->yx->setEnabled(true);
    ui->scan->setEnabled(true);
    ui->sz->setEnabled(true);
}


void MainWindow::displayDSP_slot(int num, int status)
{
    qDebug() << "DSP id: status: "<<num << ":"<< status;


    if( num == 128 && status == 1)
    {

        ui->dspok->setText("正常花盆:");
        ui->dsperror->setText("异常花盆:");
        ui->dspprocess->setText("花盆查询中");
    }

    if (num == 128 &&status == 0)
    {
        ui->tz->setEnabled(true);
        ui->dspprocess->setText("此轮查询完毕！");

        //系统首次启动，在第一次查询dsp结束后发送dtu信息
       // updateDTU();
        if(first)
        {
            updateDTU();
            first--;
        }
    }

    if(num == 128 && status == -1 )
    {
        ui->dspprocess->setText("所有花盆异常，正在自动修复！");
        runlog("",2,"Try to reset dspfd!");
        Uart_close(E->dspfd);
        sleep(5);
        E->dspfd = Uart_open("/dev/ttymxc1");
        Uart_485(E->dspfd);
        Uart_config(E->dspfd, bandrate);

    }

    if(num != 128 && status == 1)
    {
        ui->tz->setDisabled(true);
        ui->scan->setDisabled(true);
        ui->dspprocess->setText("正在查询:"+QString::number(num,10));
    }
    if (num != 128 && status == 0)
    {
        ui->dspok->setText(ui->dspok->text()+QString::number(num,10)+" ");
        fillTable();
    }

    if (num != 128 && status == -1)
        ui->dsperror->setText(ui->dsperror->text()+QString::number(num,10)+" ");

}

void MainWindow::fillTable()
{
    ui->p0->setText(QString::number(R->param[0],10));
    ui->p1->setText(QString::number(R->param[1],10));
    ui->p2->setText(QString::number(R->param[2],10));
    ui->p3->setText(QString::number(R->param[3],10));
    ui->p4->setText(QString::number(R->param[4],10));
    ui->p5->setText(QString::number(R->param[5],10));
    ui->p6->setText(QString::number(R->param[6],10));
    ui->p7->setText(QString::number(R->param[7],10));
    ui->p8->setText(QString::number(R->param[8],10));
    ui->p9->setText(QString::number(R->param[9],10));
    ui->p10->setText(QString::number(R->param[10],10));
    ui->p11->setText(QString::number(R->param[11],10));

    ui->s0->setText(QString::number(R->status[0],10));
    ui->s1->setText(QString::number(R->status[1],10));
    ui->s2->setText(QString::number(R->status[2],10));
    ui->s3->setText(QString::number(R->status[3],10));
    ui->s4->setText(QString::number(R->status[4],10));
    ui->s5->setText(QString::number(R->status[5],10));
    ui->s6->setText(QString::number(R->status[6],10));
}

void MainWindow::displayDTU_slot(int dtu,int status)
{
    switch(status)
    {
        case 0:
            ui->dtustatus->setText("上报成功");
            break;
        case -1:
            ui->dtustatus->setText("上报失败");
            break;
        default:
            ui->dtuprocess->setText("尝试第" + QString::number(status,10) +"次上报");
            ui->dtustatus->setText("正在上报");
            break;
    }
}

void MainWindow::refreshTime_slot()
{
    time(&t);

    struct tm *tmp;
    tmp = gmtime(&t);

    char buf[64];
    if(strftime(buf,64,"%T %F",tmp) == 0)
        printf("buffer 64 is too small\n");
    else
        ui->clock->setText(buf);
}

void MainWindow::updateDTU()
{

    dtuthd->start();
}

void MainWindow::queryDSP()
{
    dspthd->start();
}


void MainWindow::showSZ()
{
    sz.show();
    this->hide();
}

void MainWindow::showCSB()
{
   // csb = new canshubiao();
    csb.showtable(10);
    csb.show();
    this->hide();
}

void MainWindow::hideSZ()
{
    sz.hide();
    this->show();
}

void MainWindow::hideCSB()
{
  //  csb->hide();
    this->show();
}

void MainWindow::showZTB()
{
    ztb.showtable(10);
    ztb.show();
    this->hide();
}

void MainWindow::hideZTB()
{
    ztb.hide();
    this->show();
}

void MainWindow::softExitRestart()
{
    //this->close();
    runlog("",2,"Reboot!");
    QProcess::execute("reboot");

}


MainWindow::~MainWindow()
{
    delete ui;
    Uart_close(E->dspfd);
    Uart_close(E->dtufd);
}

void MainWindow::on_resetdb_clicked()
{
    resetDB(E);
}

void MainWindow::on_scan_clicked()
{
    runlog("",2,"Scan DSP: %d",devices);
    ui->yx->setDisabled(true);
    ui->tz->setDisabled(true);
    ui->sz->setDisabled(true);
    ui->scan->setDisabled(true);

    ui->dspprocess->setText("花盆扫描中");
    ui->dspok->setText("正常花盆：");
    ui->dsperror->setText("异常花盆：");
   scanthd->start();
}



void MainWindow::scanDSP_slot(int num,int res)
{
    QString n = QString::number(num,10);
    if(res == 0)
        ui->dspok->setText(ui->dspok->text()+" " + n);
    if(res == -1)
        ui->dsperror->setText(ui->dsperror->text() +  " " + n);
    if(res == 1)
    {
        ui->dspok->setText("正常花盆：");
        ui->dsperror->setText("异常花盆：");
        ui->dspprocess->setText("开始扫描！");
    }
    if(res == 2)
    {
        ui->dspprocess->setText("扫描完成！");
        ui->yx->setEnabled(true);
        ui->tz->setEnabled(true);
        ui->sz->setEnabled(true);
        ui->scan->setEnabled(true);
        runlog("",2,"Scan END");
        qDebug() << "Scan END";
    }


}

void MainWindow::on_uploadlog_clicked()
{
     //QProcess::execute("ls -t /root/ |grep 'log' | head -n 1 | xargs tar -cf /root/upload.gzip");
   // updateLog(E);
   // initlocaltime(E);
    testdtu();

}
