#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int fontId = QFontDatabase::addApplicationFont(":/fonts/fonts/DMSerifDisplay-Regular.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load DM Serif Display font";
    } else {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            qDebug() << "Loaded font family:" << fontFamilies.at(0);
        }
    }
    QTranslator translator;
    if (translator.load(":/translations/translations/language_en.qm")) {
        a.installTranslator(&translator);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
