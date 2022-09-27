#include "longmath_calculator/main_window.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app{argc, argv};

#if WIN32 || WIN64
    QApplication::setStyle("fusion");
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif

    MainWindow window{"Long Math Calculator"};
    window.setFixedSize(window.size());
    window.show();

    return QApplication::exec();
}
