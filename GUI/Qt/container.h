#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>

class Container : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(QString name MEMBER m_name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)

public:
    explicit Container(QObject *parent = 0);
    //explicit Container(QString name, QObject *parent = 0);
    ~Container();

    QString getName() const;

signals:
    void nameChanged(const QString &name);

public slots:
    void setName(const QString& name);

private:
    QString name_;

};

class ContainerListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ContainerListModel(QObject *parent = 0);

private:
    QList<Container> containerList_;

};

#endif // CONTAINER_H
