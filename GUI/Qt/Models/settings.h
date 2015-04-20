#pragma once


#include <QtGui>
#include <QObject>
#include <QSettings>


class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(QObject* parent = 0);
    Settings(Format format, Scope scope, const QString& organization, const QString& application = QString(), QObject * parent = 0);
    ~Settings();

    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    static std::string containerLocation();
    static std::string vhdLocation();
signals:
  void settingChanged(const QString& key);
};
