#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->show();

    return a.exec();
}
