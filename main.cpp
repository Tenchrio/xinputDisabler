#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Tenchrio");
    QCoreApplication::setApplicationName("XInputDisabler");
    QSettings settings;
    MainWindow w;
    w.show();

    return a.exec();
}
