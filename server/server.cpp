#include "server.h"

Server::Server()
{
    if (this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "error";
    }
    nextBlockSize = 0;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &Server::deleteLater);

    socketsVec.push_back(socket);
    qDebug() << "client connected" << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket=(QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        for (;;)
        {
            qDebug() << "nextBlockSize = 0";
            if (nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                {
                    qDebug() << "dataByteArray < 2; break";
                    break;
                }
                in>>nextBlockSize;
                qDebug() << "nextBlockSize... " << nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "dataByteArray not full! break";
                break;
            }

            QString str;
            QTime time;
            in >> time >> str;
            qDebug() << str;
            nextBlockSize = 0;
            sendToClient(str);
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::sendToClient(QString str)
{
    dataByteArray.clear();
    QDataStream out(&dataByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << uint16_t(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << uint16_t(dataByteArray.size() - sizeof(uint16_t));
    for (auto it: socketsVec)
    {
        it->write(dataByteArray);
    }
}
