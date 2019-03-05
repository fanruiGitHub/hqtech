#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>

class NetManager : public QObject
{
    Q_OBJECT
public:
    explicit NetManager(QObject *parent = 0);

signals:
    void resultReady(const QString &result);

public slots:
    void doWork();
};

#endif // NETMANAGER_H
