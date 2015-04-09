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

QString ContainerObject::history() const
{
    return history_;
}

void ContainerObject::setHistory(const QString& entry)
{
    if (entry.length())
        history_.append(entry + "\n\n");
    else
        history_ = QString();
    emit historyChanged();
}

bool ContainerObject::exportHistory(const QString& url)
{
    // Move to backend, using poco++
    if (url.isEmpty())
        return false;

    QFile file(url);
    // file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    QTextStream out(&file);
    out << "data";
    file.close();


    return true;
}

bool ContainerObject::encrypt(const QString& password)
{
    // TODO
    // if password == password_
    // setEncrypted(false)

    return false;
}

void ContainerObject::openDirectory(const QString& url)
{
    if (url.length())
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    else
        QDesktopServices::openUrl(QUrl(path_, QUrl::TolerantMode));

}





ContainerModel::ContainerModel(QObject* parent)
    : QAbstractListModel(parent)
{
    add(new ContainerObject("Container0", "file:///Users/jochen/Desktop", "", false));
    add(new ContainerObject("Container1", "file:///Users/jochen/Desktop", "", true));
    add(new ContainerObject("Container2", "file:///Users/jochen/Desktop", "", true));
    add(new ContainerObject("Container3", "file:///Users/jochen/Desktop", "", false));

    // TODO: load container on startup
}

ContainerModel::~ContainerModel()
{ }

int ContainerModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return containerList_.size(); //.count()
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

bool ContainerModel::add(ContainerObject* container)
{
    if (contains(container))
        return false;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    containerList_.append(container);
    endInsertRows();
    emit countChanged();
    return true;
}

bool ContainerModel::add(const QString& name, const QString& path, const QString& password, bool encrypted)
{
    return add(new ContainerObject(name, path, password, encrypted));
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

void ContainerModel::setCurrentContainer(int idx)
{
    if (idx < 0 || idx >= containerList_.size())
        currentContainer_ = NULL;
    else
        currentContainer_ = containerList_.at(idx);
}

ContainerObject* ContainerModel::currentContainer()
{
    return currentContainer_;
}

bool ContainerModel::contains(ContainerObject *container)
{
    QListIterator<ContainerObject*> iter(containerList_);
    while (iter.hasNext())
    {
        if (container->name() == iter.next()->name())
            return true;
    }
    return false;
}
