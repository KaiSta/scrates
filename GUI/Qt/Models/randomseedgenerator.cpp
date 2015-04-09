#include "randomseedgenerator.h"



RandomSeedGenerator::RandomSeedGenerator()
{ }

RandomSeedGenerator::~RandomSeedGenerator()
{ }





RandomSeedGeneratorModel::RandomSeedGeneratorModel(QObject *parent)
    : QObject(parent)
{
    // TODO: receive data from backend
    seed_ = QByteArray("hello", 32);
}

RandomSeedGeneratorModel::~RandomSeedGeneratorModel()
{ }

void RandomSeedGeneratorModel::randomSeed(double x, double y)
{
    union {
        double d;
        unsigned char b[sizeof(double)];
    } pos;
    pos.d = x;

    /* pseudo
    unsigned char byte1 = y & 0xFF;
    unsigned char byte2 = (y & 0xFF00) >> 1;
    seed[x%size] ^= byte1;
    ++x;
    seed[x%size] ^= byte2;
    */

    // TODO
    int i = (int)x % seed_.size();
    seed_[i] = seed_[i] + y;

    emit seedChanged();
}

void RandomSeedGeneratorModel::time()
{
    // TODO
    seed_[0] = seed_[0] + 1;
    emit seedChanged();
}

QString RandomSeedGeneratorModel::seed() const
{
    return QString(seed_.toBase64());
}

