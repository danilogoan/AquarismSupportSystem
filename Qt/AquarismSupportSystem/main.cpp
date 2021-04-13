#include "dialog.h"
#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <QDir>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    QDir dir;

    w.setFixedSize(670,475);
    w.setWindowTitle("Aquarism Support System");
    qDebug() << dir.currentPath();
    w.setWindowIcon(QIcon(dir.currentPath() + "/Images/pngLogo.jpg"));
    w.show();

    return a.exec();
}
