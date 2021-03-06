#include "shezhi.h"
#include "ui_shezhi.h"
#include <termios.h>

shezhi::shezhi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::shezhi)
{
    ui->setupUi(this);
    s = new QSettings("/opt/rtu/config.ini", QSettings::IniFormat);
    //ui->devices->setText(s->value("devices").toString());
    ui->devices->setText(QString::number(E->ini.devices,10));

    dsprate =  s->value("dsprate").toInt();
    switch(E->ini.dsprate)
    {
        case B9600:
            ui->rate1->setChecked(true);
            break;
        case B57600:
            ui->rate2->setChecked(true);
            break;
        case B115200:
            ui->rate3->setChecked(true);
            break;
        case B460800:
            ui->rate4->setChecked(true);
        break;
    }

    switch (E->ini.autodsp) {
        case 1:
            ui->autodspt->setChecked(true);
            break;
        case 0:
            ui->autodspf->setChecked(true);
            break;
    }


    ui->dsptimer->setText(s->value("dsptimer").toString());
    ui->dtutimer->setText(s->value("dtutimer").toString());

    connect(ui->autodspf,SIGNAL(clicked()),this,SLOT(on_autodspf_clicked()));
    connect(ui->autodspt,SIGNAL(clicked()),this,SLOT(on_autodspt_clicked()));

    connect(ui->rate1,SIGNAL(clicked()),this,SLOT(on_rate1_clicked()));
    connect(ui->rate2,SIGNAL(clicked()),this,SLOT(on_rate2_clicked()));
    connect(ui->rate3,SIGNAL(clicked()),this,SLOT(on_rate3_clicked()));
    connect(ui->rate4,SIGNAL(clicked()),this,SLOT(on_rate4_clicked()));

    connect(ui->save,SIGNAL(clicked()),this,SLOT(saveSZ()));
    connect(ui->cancel,SIGNAL(clicked()),this,SLOT(cancelSZ()));

 //   runlog("",0,"设置变更：Devices:%d DSPTimer:%d分钟 DTUTimer:%d小时",
 //          s->value("devices").toInt(),s->value("dsptimer").toInt(),s->value("dtutimer").toInt());

}

shezhi::~shezhi()
{
    delete ui;
}
void shezhi::saveSZ()
{
  //  set->setValue("rate",rate.text());
    s->setValue("devices",ui->devices->text());
    s->setValue("dsprate",dsprate);
    s->setValue("dsptimer",ui->dsptimer->text());
    s->setValue("autodsp",autodsp);
    s->setValue("dtutimer",ui->dtutimer->text());

    E->ini.devices = ui->devices->text().toInt();
    E->ini.dsptimer = ui->devices->text().toInt();
    E->ini.dtutimer = ui->devices->text().toInt();
    E->ini.autodsp = ui->devices->text().toInt();
    emit quitSZ();
}

void shezhi::cancelSZ()
{
    emit quitSZ();
}

void shezhi::on_rate1_clicked()
{
    ui->rate1->setChecked(true);
    dsprate = 9600;
}
void shezhi::on_rate2_clicked()
{
    ui->rate2->setChecked(true);
    dsprate = 57600;
}
void shezhi::on_rate3_clicked()
{
    ui->rate3->setChecked(true);
    dsprate = 115200;
}
void shezhi::on_rate4_clicked()
{
    ui->rate4->setChecked(true);
    dsprate = 460800;
}
void shezhi::on_autodspf_clicked()
{
    ui->autodspf->setChecked(true);
    autodsp = 0;
}

void shezhi::on_autodspt_clicked()
{
    ui->autodspt->setChecked(true);
    autodsp = 1;
}

void shezhi::on_scan_clicked()
{
}
