#include "passwordstrengthchecker.h"

//const std::string PasswordStrengthChecker::ALPHABETIC_LOWER = "abcdefghijklmnopqrstuvwxyz";

PasswordStrengthChecker::PasswordStrengthChecker()
{ }

PasswordStrengthChecker::~PasswordStrengthChecker()
{ }

size_t PasswordStrengthChecker::calculateEntropy(size_t possibilities, size_t length)
{
    return ceil(log2(possibilities)) * length;
}

size_t PasswordStrengthChecker::calculatePossibilities(const std::string& pw)
{
    std::string alphabetic_lower("abcdefghijklmnopqrstuvwxyz");
    std::string alphabetic_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    std::string digits("0123456789");
    std::string special_chars("!$%&/()=?+-_,.#<>");
    bool isLower = false;
    bool isUpper = false;
    bool isDigit = false;
    bool isSpecialchar = false;
    size_t possibilities = 0;

    for (auto& c : pw)
    {
        auto lower_found = alphabetic_lower.find(c);
        auto upper_found = alphabetic_upper.find(c);
        auto digits_found = digits.find(c);
        auto special_chars_found = special_chars.find(c);

        if (!isLower && lower_found != std::string::npos)
        {
            isLower = true;
            possibilities += alphabetic_lower.length();
        }
        else if (!isUpper && upper_found != std::string::npos)
        {
            isUpper = true;
            possibilities += alphabetic_upper.length();
        }
        else if (!isDigit && digits_found != std::string::npos)
        {
            isDigit = true;
            possibilities += digits.length();
        }
        else if (!isSpecialchar && special_chars_found != std::string::npos)
        {
            isSpecialchar = true;
            possibilities += special_chars.length();
        }
    }

    return possibilities;
}

double PasswordStrengthChecker::calculateNormalizedStrength(const QString &str)
{
    // Notice: no (german) accents in utf8 (use std::wstring and utf16)
    std::string pw(str.simplified().replace(" ", "").toUtf8().constData());
    double strength = 0.0;

    if (pw.length())
    {
        auto possibilities = calculatePossibilities(pw);
        auto entropy = calculateEntropy(possibilities, pw.length());
        strength = normalizeEntropy(entropy);
    }

    return strength;
}

double PasswordStrengthChecker::normalizeEntropy(double entropy, double maximum)
{
    double val = entropy / maximum;
    return (val > 1.0) ? 1.0 : val;
}



PasswordStrengthCheckerModel::PasswordStrengthCheckerModel(QObject *parent)
    : QObject(parent), strength_(0.0), message_("Weak"), color_("red")
{ }

PasswordStrengthCheckerModel::~PasswordStrengthCheckerModel()
{ }

void PasswordStrengthCheckerModel::calcStrength(const QString &str)
{
    PasswordStrengthChecker checker;
    strength_ = checker.calculateNormalizedStrength(str);

    if (strength_ > 0.8)
    {
        message_ = "Recommended";
        color_ = "green";
    }
    else if (strength_ > 0.6)
    {
        message_ = "Strong";
        color_ = "yellowgreen";
    }
    else if (strength_ > 0.4)
    {
        message_ = "Medium";
        color_ = "yellow";
    }
    else if (strength_ > 0.2)
    {
        message_ = "Normal";
        color_ = "orange";
    }
    else
    {
        message_ = "Weak";
        color_ = "red";
    }

    emit PasswordStrengthCheckerModel::strengthChanged();
    emit PasswordStrengthCheckerModel::messageChanged();
    emit PasswordStrengthCheckerModel::colorChanged();

    strength_ = 0.0;
    message_ = "Weak";
    color_ = "red";
}

double PasswordStrengthCheckerModel::strength() const
{
    return strength_;
}

QString PasswordStrengthCheckerModel::message() const
{
    return message_;
}

QColor PasswordStrengthCheckerModel::color() const
{
    return color_;
}
