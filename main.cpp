#include "aiwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AIWidget w;
    QObject::connect(&a, &QCoreApplication::aboutToQuit, &w, &AIWidget::handleAppExit);
    w.show();
    return a.exec();
}
