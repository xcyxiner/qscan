#include <QApplication>

#include "app/ApplicationController.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ApplicationController controller;
    MainWindow window(&controller);
    window.show();

    return app.exec();
}
