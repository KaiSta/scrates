#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "passwordstrengthchecker.h"
#include "container.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    //qmlRegisterType<Container>("io.tempest.Container", 1, 0, "Container");

    // Container ListModel
    ContainerModel containerModel;
    engine.rootContext()->setContextProperty("_containerModel", &containerModel);

    // Password Strength Checker
    PasswordStrengthChecker pwStrengthChecker;
    engine.rootContext()->setContextProperty("_pwStrengthChecker", &pwStrengthChecker);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
