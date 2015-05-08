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
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(QString history READ history WRITE setHistory NOTIFY historyChanged)
public:
    ContainerObject(QObject* parent = 0);
    ContainerObject(const QString& name,
                    const QString& syncLocation = QString(),
                    QObject* parent = 0);
    ~ContainerObject();

    QString name() const;
    QString path() const;
    bool isOpen() const;

    QString history() const;
    void setHistory(const QString& entry = QString());

    void create(const QString& password, bool mount, local_file::storage_type storeType = local_file::storage_type::FOLDER, size_t storeSize = 0);
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
    QString syncLocation_;
    QString history_;
    ContainerController controller_; // libcrypt, contains and controlls mounted container
    void callbackFunc(container_event e);
};
