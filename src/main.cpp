#include <QApplication>
#include "MainWindow.h"
#include <QScreen>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow a;
    a.resize(1625,1219);
    a.move(QApplication::screens().at(0)->geometry().center() - a.frameGeometry().center());
    a.show();
    return QApplication::exec();
}
