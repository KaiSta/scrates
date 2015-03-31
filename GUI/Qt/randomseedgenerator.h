#pragma once

#include <QObject>

class RandomSeedGenerator
{

public:
    RandomSeedGenerator();
    ~RandomSeedGenerator();
};

/*
class RandomSeedGeneratorModel : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QString seed READ seed WRITE setSeed NOTIFY seedChanged)
public:
    RandomSeedGeneratorModel(QObject* parent = 0);
    ~RandomSeedGeneratorModel();
signals:
    //void seedChanged();
private:

};
*/
