#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QStringListModel>

#include "passwordstrengthchecker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    //qmlRegisterType<Container>("io.tempest.Container", 1, 0, "Container");


    QStringListModel model;

    QStringList list;
    list << "one" << "assd" << "asda";
    model.setStringList(list);

    engine.rootContext()->setContextProperty("containerModel", &model);



    // Password Strength Checker
    PasswordStrengthChecker pwStrengthChecker;
    engine.rootContext()->setContextProperty("_pwStrengthChecker", &pwStrengthChecker);



    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
