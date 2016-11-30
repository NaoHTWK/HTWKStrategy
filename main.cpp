#include <iostream>

#include <QApplication>
#include <QWidget>

#include "teamstrategycontroller.h"

int main(int argc, char *argv[]) {
    std::string teamStrategyName = "robocup";

    QApplication app(argc, argv);
    QWidget window;
    TeamStrategyController tsc(teamStrategyName, &window);
    window.show();
    return app.exec();
}
