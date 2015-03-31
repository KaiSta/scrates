#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSettings>
#include "passwordstrengthchecker.h"
#include "container.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // app.setOrganizationName("TODO");
    // app.setOrganizationDomain("TODO");
    // app.setApplicationName("TODO");

    QCoreApplication::setOrganizationName("tempest");
    QCoreApplication::setOrganizationDomain("tempest.com");
    QCoreApplication::setApplicationName("tempest GUI");

    // Model example: http://www.vladest.org/qttipsandtricks/qsettings-and-qml.html
    QSettings settings;
    engine.rootContext()->setContextProperty("_settings", &settings);

    ContainerModel containerModel;
    engine.rootContext()->setContextProperty("_containerModel", &containerModel);

    PasswordStrengthCheckerModel pwStrengthCheckerModel;
    engine.rootContext()->setContextProperty("_pwStrengthCheckerModel", &pwStrengthCheckerModel);

    //qmlRegisterType<Container>("io.tempest.Container", 1, 0, "Container");
    qmlRegisterType<ContainerObject>("tempest.Container", 1, 0, "Container");


    //#ifdef Q_OS_OSX
    //#endif
    // #ifdef Q_OS_WIN
    // #ifdef Q_OS_LINUX

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
