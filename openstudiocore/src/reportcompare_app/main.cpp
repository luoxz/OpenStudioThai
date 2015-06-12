#include "menuwindow.h"
#include "comparedialog.h"
#include <QApplication>
#include <QDebug>

//#define ONLY_COMPARE_DIALOG_START

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "argc=" <<argc;
#ifdef ONLY_COMPARE_DIALOG_START
    CompareDialog w;
    w.SetParam(argc, argv);
#else
    MenuWindow w;
#endif
    w.show();
    return a.exec();
}
