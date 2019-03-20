#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "QtSoftKeyboard/frminput.h"
#include "QtSoftKeyboard/frmnum.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*
    int ret = -1;
    do
    {
        QFont font("fangsong", 16);

        //在main函数中就加载输入法面板,保证整个应用程序可以使用输入法
        //以下方法打开数字键盘
        frmNum::Instance()->Init("lightgray",10);
        a.setFont(font);
        MainWindow w;
        w.show();

        ret = a.exec();

     }while(ret != 0);

     return ret;
*/


     QFont font("fangsong", 16);

     //在main函数中就加载输入法面板,保证整个应用程序可以使用输入法
     //以下方法打开数字键盘
     frmNum::Instance()->Init("lightgray",10);
     a.setFont(font);
    MainWindow w;
    w.show();

    return a.exec();

}
