#include "container.h"

Container::Container(const QString &name, const QString &path, bool isEncrypted, bool isSaved)
    : name_(name), path_(path), isEncrypted_(isEncrypted), isSaved_(isSaved)
{
}

Container::~Container()
{
}

QString Container::name() const
{
    return name_;
}

QString Container::path() const
{
    return path_;
}

bool Container::isEncrypted() const
{
    return isEncrypted_;
}




ContainerModel::ContainerModel(QObject *parent)
    : QAbstractListModel(parent)
{
    addContainer("foo", "bar");
    addContainer("sweet cat", "bar");
    addContainer("hot dog (encrypted)", "bar", true);
}

ContainerModel::~ContainerModel()
{
}

void ContainerModel::addContainer(const Container &container)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    containerList_ << container;
    endInsertRows();
}

void ContainerModel::addContainer(const QString& name, const QString& path, bool isEncrypted, bool isSaved)
{
    addContainer(Container(name, path, isEncrypted, isSaved));
}



void ContainerModel::removeContainer(const int row)
{
    if (row < 0 || row >= containerList_.count())
       return;

   beginRemoveRows(QModelIndex(), row, row);

   //Models::ListItem *item = containerList_.takeAt(row);
   //delete item;
   //item = NULL;

   containerList_.removeAt(row);
   endRemoveRows();
   emit (countChanged(rowCount()));
}


int ContainerModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return containerList_.count();
}

QVariant ContainerModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= containerList_.count())
        return QVariant();

    const Container &container = containerList_[index.row()];
    if (role == NameRole)
        return container.name();
    else if (role == PathRole)
        return container.path();
    return QVariant();
}

QVariant ContainerModel::get(const int row, int role) const
{
    if (row < 0 || row >= containerList_.count())
        return QVariant();

    const Container &container = containerList_[row];
    return container.name();
}



QHash<int, QByteArray> ContainerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    return roles;
}
