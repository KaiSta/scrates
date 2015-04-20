#pragma once

#include <QtGui>
#include <QObject>
#include <vector>
#include "ContainerController.h"
#include "settings.h"

class RandomSeedGeneratorModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString seed READ seed NOTIFY seedChanged)
public:
    explicit RandomSeedGeneratorModel(QObject *parent = 0);
    ~RandomSeedGeneratorModel();
    Q_INVOKABLE void setSeed();
    Q_INVOKABLE void randomSeed(double x, double y);
    Q_INVOKABLE void time();
    QString seed() const;
signals:
    void seedChanged();
private:
    std::vector<unsigned char> seed_;
    size_t time_;
    void callbackFunc(container_event e);
    ContainerController controller_;
};
