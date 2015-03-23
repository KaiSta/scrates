#ifndef CONTAINER_H
#define CONTAINER_H

#include <QAbstractListModel>
#include <QMap>
#include <QDebug>

class Container
{
public:
    Container(const QString &name, const QString &path, bool isEncrypted = false, bool isSaved = false);
    ~Container();

    QString name() const;
    QString path() const;
    bool isEncrypted() const;

private:
    QString name_;
    QString path_;
    bool isEncrypted_;
    bool isSaved_;
};

class ContainerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum ContainerRoles {
        NameRole = Qt::UserRole + 1,
        PathRole
    };

    ContainerModel(QObject *parent = 0);
    ~ContainerModel();

    void addContainer(const Container &container);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;




public slots:
    void addContainer(const QString& name, const QString& path, bool isEncrypted = false, bool isSaved = false);
    void removeContainer(const int row);
    QVariant get(const int row, int role) const;

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Container> containerList_;

    QObjectList qObjectList_;
    QList<QObject*> qObjectList2_;
    QList<Container*> qObjectList3_;

    QMap<QString, Container> containerMap_;

signals :
    void countChanged(int);

};

#endif // CONTAINER_H
