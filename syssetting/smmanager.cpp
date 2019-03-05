#include "smmanager.h"
#include <QDataStream>

SmManager::SmManager(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(dataUpdated(QByteArray)),this,SLOT(handlDataUpdated(QByteArray)));

    port = 8500;
    serverIP = new QHostAddress("192.168.0.106");
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

    shareHelper = new ShareHelper(99);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(checkData()));
}

void SmManager::sendData()
{
    timer->start(3000);
}

void SmManager::dataReceived()
{
    QDataStream in(&sData,QIODevice::ReadOnly);
    sData.append(tcpSocket->readAll());
    in.setVersion(QDataStream::Qt_5_6); //设计数据流版本
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    //in.setByteOrder(QDataStream::BigEndian);    //设置读数据高位在前 ****
    in.setByteOrder(QDataStream::LittleEndian); //设置读数据低位在前
    quint64 streamlen = sData.length();

    if(streamlen > 0)
    {
        shareHelper->LockShare();
        shareHelper->SetSharedStr("");
        shareHelper->UnlockShare();
        emit resultReady(QString(sData));
        sData.clear();
    }

    tcpSocket->close();
}

int SmManager::sendRequestWithResults(QByteArray data)
{
    tcpSocket->abort(); //取消已有的连接
    tcpSocket->connectToHost(*serverIP,port);
    const int timeout=500;
    if(!tcpSocket->waitForConnected(timeout))
    {
        emit resultReady("server is unavailable");
        return 0;
    }
    else
    {
        emit resultReady("updating data to server");
    }

    tcpSocket->write(data);
    tcpSocket->waitForBytesWritten();

    return 1;
}

void SmManager::handlDataUpdated(QByteArray data)
{
    sendRequestWithResults(data);
}

void SmManager::checkData()
{
    shareHelper->LockShare();
    string content = shareHelper->GetSharedStr();
    shareHelper->UnlockShare();
    if(!content.empty())
    {
        content = "sw:" + content;
        emit dataUpdated(content.data());
    }
    else
    {
        emit resultReady("no data updated");
    }
}
