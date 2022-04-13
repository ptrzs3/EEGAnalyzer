#include "manager.h"
#include <QApplication>
#include <string>
#include <windows.h>
#include <iostream>
#include <QSplashScreen>
#include <QDateTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap(":/background/pics/brain.jpg");
    QSplashScreen splash(pixmap);
    splash.show();
    QString t = QDir::currentPath();
    if(t.contains(' ')) {
        QMessageBox::warning(NULL,
                             "EEG Analyzer",
                             "The software path contains spaces, "
                             "please execute in a path without spaces");
        exit(0);
    }
    Manager m;
    a.processEvents();
    m.dynplot->widget->showFullScreen();
    splash.hide();
    return a.exec();
}
