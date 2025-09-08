#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::Computer));
    w.setWindowTitle("DriverSentinel");
    w.show();
    return a.exec();
}
