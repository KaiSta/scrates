#include "containermodel.h"

ContainerModel::ContainerModel(QObject* parent)
    : QAbstractListModel(parent), controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation())
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

bool ContainerModel::create(const QString& name, const QString& password, const QString& syncLocation, bool isOpen)
{
    ContainerObject* container = new ContainerObject(name, syncLocation, isOpen);
    container->create(password);
    return add(container);
}

void ContainerModel::open(const QString& file)
{
    Poco::Path p(file.toStdString());
    add(new ContainerObject(QString::fromStdString(p.getBaseName()), QString(), false));
}

void ContainerModel::import(const QString& file)
{
    // TODO, using poco?
    // check if the used provider for the (foreign) container exists in your providers list
}

void ContainerModel::read()
{
    using Poco::Glob;
    std::string containerLocation(Settings::containerLocation());

    std::set<std::string> files;
    Glob::glob(containerLocation + "*.cco", files);

    for (std::string f : files)
    {
        Poco::Path p(f);
        add(new ContainerObject(QString::fromStdString(p.getBaseName()), QString(), false));

    }
}

void ContainerModel::addProvider(const QString& placeholder, const QString& location)
{
    controller_.add_provider(placeholder.toStdString(), location.toStdString());
}

void ContainerModel::deleteProvider(const QString& placeholder)
{
    controller_.delete_provider(placeholder.toStdString());
}

bool ContainerModel::containsProvider(const QString& placeholder)
{
    return controller_.contains_provider(placeholder.toStdString());
}

void ContainerModel::refreshProviderList()
{
    controller_.refresh_providerlist();
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

