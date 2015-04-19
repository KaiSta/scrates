#include "container.h"

ContainerObject::ContainerObject(QObject* parent) : QObject(parent)
{ }

ContainerObject::ContainerObject(const QString& name, const QString& path, const QString& password, bool isOpen, QObject* parent)
    : QObject(parent), name_(name), path_(path), isOpen_(isOpen)
{
    settings_ = new Settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName());
    std::string containerLocation(settings_->value("Container/containerLocation").toString().toStdString());
    std::string vhdLocation(settings_->value("Container/vhdLocation").toString().toStdString());

    controller_ = new ContainerController::ContainerController([this](container_event e) { callbackFunc(e); }, vhdLocation);
    controller_->create(name.toStdString(), containerLocation, password.toStdString(), "/Users/jochen/Desktop/tempest/cloud/", local_file::storage_type::FOLDER, 0);

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

void ContainerObject::callbackFunc(container_event e)
{
    // TODO
}



ContainerModel::ContainerModel(QObject* parent)
    : QAbstractListModel(parent)
{
    settings_ = new Settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName());
    std::string vhdLocation(settings_->value("Container/vhdLocation").toString().toStdString());
    controller_ = new ContainerController::ContainerController([this](container_event e) { callbackFunc(e); }, vhdLocation);

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

void ContainerModel::open(const QString& file)
{
    // TODO
}

void ContainerModel::import(const QString& file)
{
    // TODO, using poco?
    // check if the used provider for the (foreign) container exists in your providers list
}

void ContainerModel::read()
{
    using Poco::Glob;
    std::string containerLocation(settings_->value("Container/containerLocation").toString().toStdString());

    std::set<std::string> files;
    Glob::glob(containerLocation + "*.cco", files);

    for (std::string f : files)
    {
        Poco::Path p(f);
        add(QString::fromStdString(p.getBaseName()), QString::fromStdString(f));
    }
}

void ContainerModel::addProvider(const QString& placeholder, const QString& location)
{
    controller_->add_provider(placeholder.toStdString(), location.toStdString());
}

void ContainerModel::deleteProvider(const QString& placeholder)
{
    controller_->delete_provider(placeholder.toStdString());
}

bool ContainerModel::containsProvider(const QString& placeholder)
{
    return controller_->contains_provider(placeholder.toStdString());
}

void ContainerModel::refreshProviderList()
{
    controller_->refresh_providerlist();
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

void ContainerModel::callbackFunc(container_event e)
{
    // TODO
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
