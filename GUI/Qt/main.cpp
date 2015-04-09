#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSettings>
#include "Models/container.h"
#include "Models/passwordstrengthchecker.h"
#include "Models/randomseedgenerator.h"

#include "stdlib.h"

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

    engine.rootContext()->setContextProperty("_applicationDirPath", QApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("_homeDirPath", getenv("HOME"));

    // Model example: http://www.vladest.org/qttipsandtricks/qsettings-and-qml.html
    QSettings settings;
    engine.rootContext()->setContextProperty("_settings", &settings);

    ContainerModel containerModel;
    engine.rootContext()->setContextProperty("_containerModel", &containerModel);

    PasswordStrengthCheckerModel pwStrengthCheckerModel;
    engine.rootContext()->setContextProperty("_pwStrengthCheckerModel", &pwStrengthCheckerModel);

    //RandomSeedGeneratorModel randomSeedGeneratorModel;
    //engine.rootContext()->setContextProperty("_randomSeedGeneratorModel", &randomSeedGeneratorModel);

    qmlRegisterType<ContainerObject>("tempest.Container", 1, 0, "Container");
    qmlRegisterType<RandomSeedGeneratorModel>("tempest.RandomSeedGenerator", 1, 0, "RandomSeedGenerator");

    //#ifdef Q_OS_OSX
    //#endif
    // #ifdef Q_OS_WIN
    // #ifdef Q_OS_LINUX

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
