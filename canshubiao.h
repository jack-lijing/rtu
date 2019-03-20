#ifndef CANSHUBIAO_H
#define CANSHUBIAO_H

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
class canshubiao;
}

class canshubiao : public QWidget
{
    Q_OBJECT

public:
    explicit canshubiao(QWidget *parent = nullptr);
    void shownextpage();
    void showprevpage();
    void showtable(int);
    ~canshubiao();

private:
    Ui::canshubiao *ui;
    QSqlDatabase *db;
    int offset;
    int step;

public slots:
    void hideCSB();

signals:
    void quitCSB();
private slots:
    void on_next_clicked();
    void on_prev_clicked();
    void on_first_clicked();
};

#endif // CANSHUBIAO_H
