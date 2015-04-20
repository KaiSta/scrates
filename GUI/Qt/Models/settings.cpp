#include "settings.h"

Settings::Settings(QObject* parent) :
    QSettings(parent)
{ }

Settings::Settings(Format format, Scope scope, const QString& organization, const QString& application, QObject*) :
    QSettings(format, scope, organization, application)
{ }

Settings::~Settings()
{ }

void Settings::setValue(const QString& key, const QVariant& value)
{
    if (value != this->value(key))
    {
        QSettings::setValue(key, value);
        emit settingChanged(key);
    }
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

std::string Settings::containerLocation()
{
    return QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName()).value("Container/containerLocation").toString().toStdString();
}

std::string Settings::vhdLocation()
{
    return QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName()).value("Container/vhdLocation").toString().toStdString();
}
