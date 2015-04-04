#include "randomseedgenerator.h"



RandomSeedGenerator::RandomSeedGenerator()
{ }

RandomSeedGenerator::~RandomSeedGenerator()
{ }





RandomSeedGeneratorModel::RandomSeedGeneratorModel(QObject *parent)
    : QObject(parent)
{ }

RandomSeedGeneratorModel::~RandomSeedGeneratorModel()
{ }

void RandomSeedGeneratorModel::calcStrength(const QString &str)
{

}

QByteArray RandomSeedGeneratorModel::seed() const
{
    return strength_;
}

