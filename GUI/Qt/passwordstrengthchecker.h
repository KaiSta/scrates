#ifndef PASSWORDSTRENGTHCHECKER_H
#define PASSWORDSTRENGTHCHECKER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QDebug>

#include <ctype.h>
#include <math.h>
#include <algorithm>

class PasswordStrengthChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double strength READ getStrength NOTIFY strengthChanged)
    Q_PROPERTY(QString message READ getMessage NOTIFY messageChanged)
    Q_PROPERTY(QColor color READ getColor NOTIFY colorChanged)

public:
    explicit PasswordStrengthChecker(QObject *parent = 0);
    PasswordStrengthChecker(const PasswordStrengthChecker& checker) = delete;
    PasswordStrengthChecker& operator= (const PasswordStrengthChecker& checker) = delete;
    ~PasswordStrengthChecker();

    double getStrength() const;
    QString getMessage() const;
    QColor getColor() const;

signals:
    void strengthChanged();
    void messageChanged();
    void colorChanged();

public slots:
    void calcStrength(const QString &str);

private:
    // Calculates and returns the possibilities
    size_t calcPossibilities(const std::string &pw);

    // Calculates and returns the entropy
    size_t calcEntropy(const size_t possibilities, const size_t length);

    // static const std::string ALPHABETIC_LOWER;

    // Member variables
    double strength_;
    QString message_;
    QColor color_;
};

#endif // PASSWORDSTRENGTHCHECKER_H
