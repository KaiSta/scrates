#include "container.h"

ContainerObject::ContainerObject(QObject* parent)
    : QObject(parent), controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation())
{ }

ContainerObject::ContainerObject(const QString& name, const QString& syncLocation, QObject* parent)
    : name_(name), syncLocation_(syncLocation), QObject(parent), history_(QString()),
      controller_([this](container_event e) { callbackFunc(e); }, Settings::vhdLocation())
{ }

ContainerObject::~ContainerObject()
{ }

QString ContainerObject::name() const
{
    QString displayedName(name_);
    if (isOpen()) displayedName.append("*");
    return displayedName;
}


QString ContainerObject::path() const
{
    return path_;
}

bool ContainerObject::isOpen() const
{
    return controller_.is_open();
}

QString ContainerObject::history() const
{
    return history_;
}

void ContainerObject::setHistory(const QString& entry)
{
    QTime time(QTime::currentTime());

    if (entry.length())
        history_.append(time.toString("hh:mm:ss") + ": \t" + entry);
    else
        history_ = QString();
    emit historyChanged();
}

void ContainerObject::create(const QString& password, bool mount, local_file::storage_type storeType, size_t storeSize)
{
    controller_.create(name_.toStdString(), Settings::containerLocation(), password.toStdString(), this->syncLocation_.toStdString(), storeType, storeSize);

    // If the user doesnt want to automount the container after creating, demount it.
    if (!mount && controller_.is_open())
        controller_.close();

    emit openChanged();
}

bool ContainerObject::mount(const QString& password)
{
    std::string containerFullPath(Settings::containerLocation() + this->name_.toStdString() + ".cco");

    if (controller_.open(containerFullPath, password.toStdString(), local_file::storage_type::FOLDER))
    {
        emit openChanged();
        emit nameChanged();
        return true;
    }
    return false;
}

void ContainerObject::unmount()
{
    controller_.close();
    emit openChanged();
    emit nameChanged();
}

bool ContainerObject::sync()
{
    return controller_.sync_now();
}

void ContainerObject::openDirectory(const QString& url)
{
    if  (url.length())
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    else
        QDesktopServices::openUrl(QUrl("file://" + QString::fromStdString(Settings::vhdLocation()) + name_, QUrl::TolerantMode));
}

void ContainerObject::callbackFunc(container_event e)
{
    // TODO: VERBOSE = debug infos. also am besten makro rein, dass nur bei debug build das ausgegeben wird
    std::string type;
    std::string message;
    QColor color;

    switch (e.type)
    {
    case INFORMATION:
        type = "information";
        color = Qt::darkBlue;
        break;
    case CONFLICT:
        type = "error";
        color = Qt::darkRed;
        break;
    case WARNING:
        type = "warning";
        color = Qt::darkYellow;
        break;
    case VERBOSE:
        type = "verbose";
        color = Qt::darkGray;
        break;
    }

    switch (e.message)
    {
    case NONE:
        message = "none";
        break;
    case CTR_FILE_NOT_FOUND:
        message = "container file not found";
        break;
    case WRONG_PASSWORD:
        message = "wrong password";
        break;
    case MISSING_ENC_FILES:
        message = "missing encrypted file in cloud";
        break;
    case DECRYPTION_ERROR:
        message = "decryption error";
        break;
    case WRONG_ARGUMENTS:
        message = "wrong arguments";
        break;
    case SUCC:
        message = "succ";
        break;
    case SYNCHRONIZING:
        message = "sync";
        break;
    case FINISHED_SYNCHRONIZING:
        message = "finished sync";
        break;
    case NO_WARNING:
        message = "no warning";
        break;
    case CLOSING:
        message = "closing container";
        break;
    case ADD_FILE:
        message = "adding file " + std::string(e._data_.data());
        break;
    case UPDATE_FILE:
        message = "updating file " + std::string(e._data_.data());
        break;
    case DELETE_FILE:
        message = "delete file " + std::string(e._data_.data());
        break;
    case EXTRACT_FILE:
        message = "extract file " + std::string(e._data_.data());
        break;
    case EXTRACT_FILES:
        message = "extract files ";
        break;
    case INIT_CONTAINER:
        message = "initializing container";
        break;
    }
    setHistory("<span style='color: " + color.name() + "'>" + QString::fromStdString(type) + "</span>: " + QString::fromStdString(message) + "<br/>");
}
