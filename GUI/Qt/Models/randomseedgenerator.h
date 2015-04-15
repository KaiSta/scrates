#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <vector>
#include <Container.h>
#include "ContainerController.h"


class RandomSeedGenerator
{
public:
    RandomSeedGenerator();
    ~RandomSeedGenerator();

private:
};

class RandomSeedGeneratorModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString seed READ seed NOTIFY seedChanged)
public:
    explicit RandomSeedGeneratorModel(QObject *parent = 0);
    ~RandomSeedGeneratorModel();
    Q_INVOKABLE void randomSeed(double x, double y);
    //Q_INVOKABLE void mousePosition(double x, double y);
    //Q_INVOKABLE void windowPosition(double x, double y);
    Q_INVOKABLE void time();

    QString seed() const;

signals:
    void seedChanged();

private:
    std::vector<unsigned char> seed_;
    size_t time_;
    void myfunc(container_event e);
    ContainerController* controller_;
};
