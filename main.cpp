#include "mainwindow.h"
#include <QApplication>
#include <QtGui> //cout

#include "QDebugStream.h" //for redirecting cout to QTextEdit
#include <QTextEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();




/*  http://stackoverflow.com/questions/10308425/redirect-stdcout-to-a-qtextedit

    QTextEdit* myTextEdit = new QTextEdit();
    myTextEdit->setReadOnly(true);
    //myTextEdit->setTextFormat(Qt::LogText)

   // QDebugStream qout = new QDebugStream(std::cout, myTextEdit);
    QDebugStream qout(std::cout, myTextEdit);
    std::cout << "send this to Text Edit!" << endl;
*/

    return a.exec();
}
