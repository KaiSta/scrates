#include "passwordstrengthchecker.h"

//const std::string PasswordStrengthChecker::ALPHABETIC_LOWER = "abcdefghijklmnopqrstuvwxyz";

PasswordStrengthChecker::PasswordStrengthChecker(QObject *parent)
    : QObject(parent), strength_(0.0), message_("Weak"), color_("red")
{

}

PasswordStrengthChecker::~PasswordStrengthChecker()
{

}

size_t PasswordStrengthChecker::calcPossibilities(const std::string &pw)
{
    std::string alphabetic_lower("abcdefghijklmnopqrstuvwxyz");
    std::string alphabetic_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    std::string digits("0123456789");
    std::string special_chars("!$%&/()=?+-_,.#<>");

    bool islower, isupper, isdigit, isspecialchar = false;
    size_t possibilities = 0;

    for (auto& c : pw)
    {
        auto lower_found = alphabetic_lower.find(c);
        auto upper_found = alphabetic_upper.find(c);
        auto digits_found = digits.find(c);
        auto special_chars_found = special_chars.find(c);

        if (!islower && lower_found != std::string::npos)
        {
            islower = true;
            possibilities += alphabetic_lower.length();
        }
        else if (!isupper && upper_found != std::string::npos)
        {
            isupper = true;
            possibilities += alphabetic_upper.length();
        }
        else if (!isdigit && digits_found != std::string::npos)
        {
            isdigit = true;
            possibilities += digits.length();
        }
        else if (!isspecialchar && special_chars_found != std::string::npos)
        {
            isspecialchar = true;
            possibilities += special_chars.length();
        }
    }

    return possibilities;
}

size_t PasswordStrengthChecker::calcEntropy(const size_t possibilities, const size_t length)
{
    return ceil(log2(possibilities)) * length;
}

void PasswordStrengthChecker::calcStrength(const QString &str)
{
    // Notice: no (german) accents in utf8 (use std::wstring and utf16)
    std::string pw = str.simplified().replace(" ", "").toUtf8().constData();

    strength_ = 0.0;

    if (pw.length())
    {
        auto possibilities = calcPossibilities(pw);
        auto entropy = calcEntropy(possibilities, pw.length());

        // normalize
        double max = 256.0;
        double val = entropy / max;

        strength_ = (val > 1.0) ? 1.0 : val;
    }


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

    emit PasswordStrengthChecker::strengthChanged();
    emit PasswordStrengthChecker::messageChanged();
    emit PasswordStrengthChecker::colorChanged();
}

double PasswordStrengthChecker::strength() const
{
    return strength_;
}

QString PasswordStrengthChecker::getMessage() const
{
    return message_;
}

QColor PasswordStrengthChecker::getColor() const
{
    return color_;
}
