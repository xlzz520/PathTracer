#include <QApplication>

#include "Displayer.h"

int main(int argc, char **argv) {

    QApplication application(argc, argv);
    Displayer renderer;
    renderer.show();

    return QApplication::exec();
}
