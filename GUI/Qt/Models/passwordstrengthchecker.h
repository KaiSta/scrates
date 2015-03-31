#pragma once

#include <QObject>
#include <QString>
#include <QColor>
#include <ctype.h>
#include <math.h>
#include <algorithm>

class PasswordStrengthChecker
{
public:
    PasswordStrengthChecker();
    PasswordStrengthChecker(const PasswordStrengthChecker& checker) = delete;
    PasswordStrengthChecker& operator= (const PasswordStrengthChecker& checker) = delete;
    ~PasswordStrengthChecker();
    double calculateNormalizedStrength(const QString& str);
private:
    // static const std::string ALPHABETIC_LOWER;
    size_t calculatePossibilities(const std::string& pw);
    size_t calculateEntropy(size_t possibilities, size_t length);
    double normalizeEntropy(double entropy, double maximum = 256.0);
};

class PasswordStrengthCheckerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double strength READ strength NOTIFY strengthChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
public:
    explicit PasswordStrengthCheckerModel(QObject *parent = 0);
    ~PasswordStrengthCheckerModel();
    Q_INVOKABLE void calcStrength(const QString &str);
    double strength() const;
    QString message() const;
    QColor color() const;
signals:
    void strengthChanged();
    void messageChanged();
    void colorChanged();
private:
    double strength_;
    QString message_;
    QColor color_;
};
