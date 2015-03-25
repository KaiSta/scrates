#include "container.h"

ContainerObject::ContainerObject(QObject* parent) : QObject(parent)
{ }

ContainerObject::ContainerObject(const QString& name, const QString& path, const QString& password, bool encrypted, QObject* parent)
    : QObject(parent), name_(name), path_(path), password_(password), encrypted_(encrypted)
{ }

ContainerObject::~ContainerObject()
{ }

QString ContainerObject::name() const
{
    return name_;
}

void ContainerObject::setName(const QString& name)
{
    if (name != name_)
    {
        name_ = name;
        emit nameChanged();
    }
}

QString ContainerObject::path() const
{
    return path_;
}

void ContainerObject::setPath(const QString& path)
{
    if (path != path_)
    {
        path_ = path;
        emit pathChanged();
    }
}

QString ContainerObject::password() const
{
    return password_;
}

void ContainerObject::setPassword(const QString& password)
{
    if (password != password_)
    {
        password_ = password;
        emit passwordChanged();
    }
}

bool ContainerObject::isEncrypted() const
{
    return encrypted_;
}

void ContainerObject::setEncrypted(bool encrypted)
{
    if (encrypted != encrypted_)
    {
        encrypted_ = encrypted;
        emit encryptedChanged();
    }
}














ContainerModel::ContainerModel(QObject* parent)
    : QAbstractListModel(parent)
{
    add(new ContainerObject("yeah, test container"));
    add(new ContainerObject("true one", "", "", true));
}

ContainerModel::~ContainerModel()
{ }

int ContainerModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return containerList_.size();
}

QVariant ContainerModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= containerList_.size())
        return QVariant();

    if (role == ObjectRole)
        return QVariant::fromValue(containerList_.at(index.row()));

    return QVariant();
}

QHash<int, QByteArray> ContainerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ObjectRole] = "obj";
    return roles;
}

ContainerObject* ContainerModel::get(const int idx) const
{
    if (idx < 0 || idx >= containerList_.size())
        return 0;

    return containerList_.at(idx);
}

void ContainerModel::add(ContainerObject* container)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    containerList_.append(container);
    endInsertRows();
    emit countChanged();
}

void ContainerModel::add(const QString& name /*TODO*/)
{
    add(new ContainerObject(name));
}

void ContainerModel::remove(int idx)
{
    if (idx < 0 || idx >= containerList_.size())
       return;

    beginRemoveRows(QModelIndex(), idx, idx);
    ContainerObject* container = containerList_.takeAt(idx);
    delete container;
    container = NULL;
    endRemoveRows();
    emit countChanged(/*rowCount()*/);
}
