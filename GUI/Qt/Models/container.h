#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QtGui>
#include <QFile>
#include <QTextStream>

#include "Poco/Path.h"
#include "Poco/Glob.h"
#include "ContainerController.h"
#include "settings.h"

class ContainerObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(QString history READ history WRITE setHistory NOTIFY historyChanged)
public:
    ContainerObject(QObject* parent = 0);
    ContainerObject(const QString& name,
                    const QString& path = QString(),
                    const QString& password = QString(),
                    bool isOpen = true,
                    QObject* parent = 0);
    ~ContainerObject();

    QString name() const;
    void setName(const QString& name);
    QString path() const;
    void setPath(const QString& path);
    bool isOpen() const;
    QString history() const;
    void setHistory(const QString& entry = QString());

    Q_INVOKABLE bool exportHistory(const QString& url = QString());
    Q_INVOKABLE bool mount(const QString& password);
    Q_INVOKABLE void unmount();
    Q_INVOKABLE bool sync();

    Q_INVOKABLE void openDirectory(const QString& url = QString());
signals:
    void nameChanged();
    void pathChanged();
    void openChanged();
    void historyChanged();
private:
    QString name_;
    QString path_; // containerLocation
    bool isOpen_;
    QString history_;
    Settings* settings_;
    void callbackFunc(container_event e);
    ContainerController* controller_; // libcrypt, contains and controlls mounted container
};
