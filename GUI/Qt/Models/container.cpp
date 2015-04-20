#include "container.h"

ContainerObject::ContainerObject(QObject* parent)
    : QObject(parent), controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation())
{ }

ContainerObject::ContainerObject(const QString& name, const QString& syncLocation, bool isOpen, QObject* parent)
    : name_(name), syncLocation_(syncLocation), isOpen_(isOpen), QObject(parent),
      controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation())
{ }

ContainerObject::~ContainerObject()
{ }

QString ContainerObject::name() const
{
    return name_;
}

QString ContainerObject::path() const
{
    return path_;
}

bool ContainerObject::isOpen()
{
    return controller_.is_open();
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

void ContainerObject::create(const QString& password, local_file::storage_type storeType, size_t storeSize)
{
    controller_.create(name_.toStdString(), Settings::containerLocation(), password.toStdString(), "/Users/jochen/Desktop/tempest/cloud/", local_file::storage_type::FOLDER, 0);

    // If the user doesnt want to automount the container after creating, demount it.
    if (!this->isOpen() && controller_.is_open())
        controller_.close();

    emit openChanged();
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

    if (controller_.open(containerFullPath, password.toStdString(), local_file::storage_type::FOLDER))
    {
        emit openChanged();
        return true;
    }
    return false;
}

void ContainerObject::unmount()
{
    controller_.close();
    emit openChanged();
}

bool ContainerObject::sync()
{
    return controller_.sync_now();
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
