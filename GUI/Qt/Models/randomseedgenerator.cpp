#include "randomseedgenerator.h"

RandomSeedGeneratorModel::RandomSeedGeneratorModel(QObject *parent)
    : QObject(parent), controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation()), seed_(controller_.get_pseudo_seed())
{ }

RandomSeedGeneratorModel::~RandomSeedGeneratorModel()
{ }

void RandomSeedGeneratorModel::setSeed()
{
    controller_.set_seed(seed_);
}

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
    QByteArray bytes;
    for (int i = 0; i < seed_.size(); ++i)
       bytes.append(seed_[i]);

    return QString(bytes.toBase64());
}

void RandomSeedGeneratorModel::callbackFunc(container_event e)
{
    // TODO
}
