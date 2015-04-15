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
#include "ContainerController.h"


class ContainerObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool encrypted READ isEncrypted WRITE setEncrypted NOTIFY encryptedChanged)
    Q_PROPERTY(QString history READ history WRITE setHistory NOTIFY historyChanged)
public:
    ContainerObject(QObject* parent = 0);
    ContainerObject(const QString& name,
                    const QString& path = QString(),
                    const QString& password = QString(),
                    bool encrypted = true,
                    QObject* parent = 0);
    ~ContainerObject();

    QString name() const;
    void setName(const QString& name);
    QString path() const;
    void setPath(const QString& path);
    bool isEncrypted() const;
    void setEncrypted(bool encrypted);
    QString password() const;
    void setPassword(const QString& password);
    QString history() const;
    void setHistory(const QString& entry = QString());

    Q_INVOKABLE bool exportHistory(const QString& url = QString());
    Q_INVOKABLE bool encrypt(const QString& password);
    Q_INVOKABLE void openDirectory(const QString& url = QString());
signals:
    void nameChanged();
    void pathChanged();
    void passwordChanged();
    void encryptedChanged();
    void historyChanged();
private:
    QString name_;
    QString path_;
    QString password_; // possibly unnecessary
    bool encrypted_;
    QString history_;
    void myfunc(container_event e);
    ContainerController* controller_;
};

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
    Q_INVOKABLE bool add(ContainerObject* container);
    Q_INVOKABLE bool add(const QString& name,
                         const QString& path = QString(),
                         const QString& password = QString(),
                         bool encrypted = true);
    Q_INVOKABLE void remove(int idx);
    Q_INVOKABLE void read();
    // Q_INVOKABLE void import(const QString& path /*const QDir& dir*/);
    Q_INVOKABLE ContainerObject* currentContainer();
    Q_INVOKABLE void setCurrentContainer(int idx);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<ContainerObject*> containerList_;
    ContainerObject* currentContainer_;
    bool contains(ContainerObject* container);
signals :
    void countChanged(/*int*/);
};
