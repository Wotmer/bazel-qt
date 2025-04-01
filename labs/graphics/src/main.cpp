#include "../include/raycaster_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RaycasterWidget w;
    w.show();
    return a.exec();
}
