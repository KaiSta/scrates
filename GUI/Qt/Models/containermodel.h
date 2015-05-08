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

#include "container.h"

class ContainerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum ContainerRoles {
        ObjectRole = Qt::UserRole + 1
    };

    ContainerModel(QObject* parent = 0);
    ~ContainerModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE ContainerObject* get(int idx/*const QModelIndex& index*/) const;
    // Adds a container item to the container list.
    Q_INVOKABLE bool add(ContainerObject* container);
    // Removes a container item to the container list.
    Q_INVOKABLE void remove(int idx);
    // Adds a container item to the container list and creates a container on the hard drive.
    Q_INVOKABLE bool create(const QString& name, const QString& password = QString(),
        const QString& syncLocation = QString(), bool mount = true);
    Q_INVOKABLE void open(const QString& file);
    Q_INVOKABLE void import(const QString& file);
    Q_INVOKABLE void read();
    Q_INVOKABLE void addProvider(const QString& placeholder, const QString& location);
    Q_INVOKABLE void deleteProvider(const QString& placeholder);
    Q_INVOKABLE bool containsProvider(const QString& placeholder);
    Q_INVOKABLE void refreshProviderList();
    Q_INVOKABLE ContainerObject* currentContainer();
    Q_INVOKABLE void setCurrentContainer(int idx);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<ContainerObject*> containerList_;
    ContainerObject* currentContainer_;
    ContainerController controller_; // libcrypt, contains and controlls mounted container
    void callbackFunc(container_event e);
    bool contains(ContainerObject* container);
    void closeAll();
signals :
    void countChanged(/*int*/);
};
