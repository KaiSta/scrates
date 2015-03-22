#include "container.h"

Container::Container(const QString &name, const QString &path)
    : name_(name), path_(path)
{
    // read dir
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
}

ContainerModel::~ContainerModel()
{
}

void ContainerModel::addContainer(const Container &container)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    containerList_ << container;
    endInsertRows();

    //containerMap_.insert(container.name(), container);
}

void ContainerModel::addContainer(const QString& name, const QString& path)
{
    addContainer(Container(name, path));
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

    // return containerList_.size();
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
