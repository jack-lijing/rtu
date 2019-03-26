#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include    <QMainWindow>
#include    "canshubiao.h"
#include    <QKeyEvent>
#include    "uartconfig.h"
#include    "shezhi.h"
#include    "zhuangtaibiao.h"
#include    "dspthread.h"
#include    "dtuthread.h"
#include    "mythread.h"
#include	"rtu.h"
#include    <termios.h>

#include    <QCoreApplication>
#include    <QSql>
#include    <QSqlError>
#include    <QSqlDatabase>
#include    <QSqlError>
#include    <QSqlQuery>
#include    <QString>
#include    <QFile>
#include    <QDebug>
#include    <QTimer>
#include    <QPushButton>
#include    <QProcess>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit   MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void dealReport(int num);

//    Env *E ;
//    Dsp *R ;

private:
    char *logbuf;

    time_t t;
    QSettings *set;
    QTimer *dspTimer;
    QTimer *dtuTimer;
    QTimer *clockTimer;
    dspthread *dspthd;
    dtuthread *dtuthd;
    mythread *scanthd;

    Ui::MainWindow *ui;
    shezhi sz;
    canshubiao csb;
    zhuangtaibiao ztb;
    int devices;
    int first;
    int bandrate;
    void initDb();
    void fillTable();

public slots:
    void showSZ();
    void showCSB();
    void hideSZ();
    void hideCSB();
    void showZTB();
    void hideZTB();
    void softExitRestart();
    void startRTU();
    void stopRTU();
    void queryDSP();
    void updateDTU();
    void on_scan_clicked();
    void on_resetdb_clicked();
    void displayDSP_slot(int num, int status);
    void displayDTU_slot(int dtu, int status);
    void refreshTime_slot();
    void scanDSP_slot(int,int);
    void on_uploadlog_clicked();
};

#endif // MAINWINDOW_H
