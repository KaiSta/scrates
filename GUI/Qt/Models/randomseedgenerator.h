#pragma once

#include <QObject>
#include <QString>
#include <QColor>
#include <ctype.h>
#include <math.h>
#include <algorithm>
#include <QByteArray>

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
    Q_PROPERTY(QByteArray seed READ seed NOTIFY seedChanged)

public:
    explicit RandomSeedGeneratorModel(QObject *parent = 0);
    ~RandomSeedGeneratorModel();
    Q_INVOKABLE void calcStrength(const QString &str);
    QByteArray seed() const;

signals:
    void seedChanged();

private:
    QByteArray strength_;

};
