#include "mainwindow.h"

#include <QApplication>
#include <QWindow>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    //The next 2 lines are included in case this ever gets compiled against Qt5
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    //Ensures icons follow the desktop theme
    QIcon::fromTheme("document-open");

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("MysteryCollect");
    QCoreApplication::setApplicationName("DialougeAI");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "DialogueAI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    //Setup the main window
    MainWindow w;
    w.show();

    return a.exec();
}
