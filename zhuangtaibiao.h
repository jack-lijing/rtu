#ifndef ZHUANGTAIBIAO_H
#define ZHUANGTAIBIAO_H

#include <QWidget>
#include <QTableView>
#include <QDebug>
#include <QSqlQueryModel>
#include    <QSql>
#include    <QSqlError>
#include    <QSqlDatabase>
#include    <QSqlError>
#include    <QSqlQuery>

namespace Ui {
class zhuangtaibiao;
}

class zhuangtaibiao : public QWidget
{
    Q_OBJECT

public:
    explicit zhuangtaibiao(QWidget *parent = nullptr);
    void shownextpage();
    void showprevpage();
    void showtable(int);
    ~zhuangtaibiao();

private:
    Ui::zhuangtaibiao *ui;
    QSqlDatabase *db;
    int offset;
    int step;

public slots:
    void hideZTB();

signals:
    void quitZTB();
private slots:
    void on_next_clicked();
    void on_prev_clicked();
    void on_first_clicked();
};

#endif // zhuangtaibiao_H
