#ifndef SMMANAGER_H
#define SMMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <sharehelper.h>

class SmManager : public QObject
{    
    Q_OBJECT
public:
    explicit SmManager(QObject *parent = 0);

signals:
    void resultReady(const QString &result);
    void dataUpdated(QByteArray data);

public slots:
    void sendData();

protected slots:
    void dataReceived();
    void handlDataUpdated(QByteArray data);
    void checkData();

private:
    bool status;
    int port;
    QTimer *timer;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;
    quint32 StreamLength = 0;
    QByteArray sData;
    ShareHelper* shareHelper;

    int sendRequestWithResults(QByteArray data);
};

#endif // SMMANAGER_H
