#include "containermodel.h"

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

