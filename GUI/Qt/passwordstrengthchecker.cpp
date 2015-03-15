#include "passwordstrengthchecker.h"

const std::string PasswordStrengthChecker::ALPHABETIC_LOWER = "abcdefghijklmnopqrstuvwxyz";

PasswordStrengthChecker::PasswordStrengthChecker(QObject *parent) : QObject(parent)
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
    return ceil(log2(possibilities * length));
}

void PasswordStrengthChecker::check(const QString &str)
{
    if (str.length())
    {
        // ohne Umlaute, bitte beachten für spätere GUI eingabe (// mit umlaute std::wstring und utf16)
        std::string pw = str.simplified().replace(" ", "").toUtf8().constData();

        auto possibilities = calcPossibilities(pw);
        auto entropy = calcEntropy(possibilities, str.length());
    }
}

PasswordStrengthChecker::Strength PasswordStrengthChecker::calcStrength(const size_t entropy)
{
    auto strength = PasswordStrengthChecker::Strength::VERYHIGH;

    if (entropy < 64)
        strength = PasswordStrengthChecker::Strength::LOW;
    else if (entropy < 128)
        strength = PasswordStrengthChecker::Strength::MID;
    else if (entropy < 192)
        strength = PasswordStrengthChecker::Strength::HIGH;

    return strength;
}
