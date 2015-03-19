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

QHash<int, QByteArray> ContainerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    return roles;
}
