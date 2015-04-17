#include "container.h"

ContainerObject::ContainerObject(QObject* parent) : QObject(parent)
{ }

ContainerObject::ContainerObject(const QString& name, const QString& path, const QString& password, bool isOpen, QObject* parent)
    : QObject(parent), name_(name), path_(path), isOpen_(isOpen)
{
    controller_ = new ContainerController::ContainerController([this](container_event e) { myfunc(e); }, "/Users/jochen/Desktop/tempest/temp/");
    controller_->create(name.toStdString(), "/Users/jochen/Desktop/tempest/local/", password.toStdString(), "/Users/jochen/Desktop/tempest/cloud/", local_file::storage_type::FOLDER, 0);

    // If the user doesnt want to automount the container after creating, demount it.
    if (!isOpen && controller_->is_open())
        controller_->close();

    emit openChanged();
}

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

bool ContainerObject::isOpen() const
{
    return controller_->is_open();
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

bool ContainerObject::mount(const QString& password)
{
    std::string containerFullPath("/Users/jochen/Desktop/tempest/local/" + this->name_.toStdString() + ".cco");

    if (controller_->open(containerFullPath, password.toStdString(), local_file::storage_type::FOLDER))
    {
        emit openChanged();
        return true;
    }
    return false;
}

void ContainerObject::unmount()
{
    controller_->close();
    emit openChanged();
}

bool ContainerObject::sync()
{
    return controller_->sync_now();
}

void ContainerObject::openDirectory(const QString& url)
{
    if (url.length())
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    else
        QDesktopServices::openUrl(QUrl(path_, QUrl::TolerantMode));
}

void ContainerObject::myfunc(container_event e)
{
    // TODO
}



ContainerModel::ContainerModel(QObject* parent)
    : QAbstractListModel(parent)
{
    read();
}

ContainerModel::~ContainerModel()
{
    closeAll();
}

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

bool ContainerModel::add(const QString& name, const QString& path, const QString& password, bool isOpen)
{
    return add(new ContainerObject(name, path, password, isOpen));
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

void ContainerModel::read()
{
    using Poco::Glob;
    std::string containerLocation("/Users/jochen/Desktop/tempest/local/");

    std::set<std::string> files;
    Glob::glob(containerLocation +"*.cco", files);

    for (std::string f : files)
    {
        Poco::Path p(f);
        add(QString::fromStdString(p.getBaseName()), QString::fromStdString(f));
    }
}

void ContainerModel::closeAll()
{
    for (auto c : containerList_)
    {
        c->sync();
        c->unmount();
    }
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
