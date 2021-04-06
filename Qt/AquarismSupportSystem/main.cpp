#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setFixedSize(670,475);
    w.setWindowTitle("Aquarism Support System");
    w.show();

    return a.exec();
}
