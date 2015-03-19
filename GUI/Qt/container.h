#ifndef CONTAINER_H
#define CONTAINER_H

#include <QAbstractListModel>
#include <QStringList>
#include <QMap>
#include <QDebug>

class Container
{
public:
    Container(const QString &name, const QString &path);
    ~Container();

    QString name() const;
    QString path() const;

private:
    QString name_;
    QString path_;
};

class ContainerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ContainerRoles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };

    ContainerModel(QObject *parent = 0);
    ~ContainerModel();

    void addContainer(const Container &container);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

public slots:
    void addContainer(const QString& name, const QString& path);

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Container> containerList_;
    QMap<QString, Container> containerMap_;
};

#endif // CONTAINER_H
