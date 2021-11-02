#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /** If the tray is available, does not close the application by clicking the close icon, but close it to tray */
#ifndef QT_NO_SYSTEMTRAYICON
    if(QSystemTrayIcon::isSystemTrayAvailable())
    {
        QApplication::setQuitOnLastWindowClosed(false);
    }
#endif // QT_NO_SYSTEMTRAYICON

    MainWindow w;
    w.show();
    return a.exec();
}
