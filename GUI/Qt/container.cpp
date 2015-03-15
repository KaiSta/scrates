#include "container.h"

Container::Container(QObject *parent) : QObject(parent)
{

}

Container::~Container()
{

}

QString Container::getName() const
{
    return name_;
}

void Container::setName(const QString& name)
{
    if (name_ != name)
    {
        name_ = name;
        emit nameChanged(name);
    }
}
