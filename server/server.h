#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QTime>

#include <vector>


class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    QTcpSocket *socket;
private:
    std::vector <QTcpSocket*> socketsVec;
    QByteArray dataByteArray;
    void sendToClient(QString str);
    uint16_t nextBlockSize;
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

