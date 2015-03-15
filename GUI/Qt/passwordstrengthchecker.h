#ifndef PASSWORDSTRENGTHCHECKER_H
#define PASSWORDSTRENGTHCHECKER_H

#include <QObject>
#include <QString>
#include <QDebug>

#include <ctype.h>
#include <math.h>
#include <algorithm>

class PasswordStrengthChecker : public QObject
{
    Q_OBJECT
public:
    explicit PasswordStrengthChecker(QObject *parent = 0);
    PasswordStrengthChecker(const PasswordStrengthChecker& checker) = delete;
    PasswordStrengthChecker& operator= (const PasswordStrengthChecker& checker) = delete;
    ~PasswordStrengthChecker();

    enum Strength : size_t
    {
        LOW,
        MID,
        HIGH,
        VERYHIGH
    };

signals:

public slots:
    void check(const QString &str);

private:
    // Calculates and returns the possibilities
    size_t calcPossibilities(const std::string &pw);

    // Calculates and returns the entropy
    size_t calcEntropy(const size_t possibilities, const size_t length);

    Strength calcStrength(const size_t entropy);

    // double normalize(size_t entropy);

    static const std::string ALPHABETIC_LOWER;
};

#endif // PASSWORDSTRENGTHCHECKER_H
