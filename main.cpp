#include "mainwindow.h"
#include "usersettings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UserSettings::getInstance()->loadSettings();
    MainWindow w;
    w.show();
    return a.exec();
}
