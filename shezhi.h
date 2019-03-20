#ifndef SHEZHI_H
#define SHEZHI_H
#include <QSettings>
#include <QWidget>
#include "mythread.h"
#include "rtu.h"
//#include "csapp.h"

namespace Ui {
class shezhi;
}

class shezhi : public QWidget
{
    Q_OBJECT
private:
    Ui::shezhi *ui;
    QSettings   *s;
    int rate;
    int autodsp;
    Env *E;
    Dsp *R;

public:
    explicit shezhi(QWidget *parent = nullptr);
    ~shezhi();

signals:
    void quitSZ();

public slots:
    void saveSZ();
    void cancelSZ();

protected:
private slots:
    void on_rate1_clicked();
    void on_rate2_clicked();
    void on_rate3_clicked();
    void on_rate4_clicked();
    void on_scan_clicked();
    void on_autodspt_clicked();
    void on_autodspf_clicked();
};

#endif // SHEZHI_H
