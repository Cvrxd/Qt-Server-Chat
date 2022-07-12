#include "server.h"

Server::Server(const int& server_port)
    : _port(server_port), _blockSize(0)
{
    if(this->listen(QHostAddress::Any, server_port))
    {
        qDebug() << "SERVER::start \n";
    }
    else
    {
        qDebug() << "SERVER::start error \n";
    }
}

Server::~Server()
{
    for(auto& el : this->_sockets)
    {
        el->disconnect();
        delete el;
        el = nullptr;
    }
}

const int &Server::getPort() const
{
    return this->_port;
}

//Slot for incoming connections
void Server::incomingConnection(qintptr socketDescriptor)
{
    //Creating server socket
    this->socket = new QTcpSocket();

    this->socket->setSocketDescriptor(socketDescriptor);

    //Connecting socket
    QObject::connect(this->socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    QObject::connect(this->socket, &QTcpSocket::disconnected, this->socket, &QTcpSocket::deleteLater);

    //Adding socket to vector container
    this->_sockets.push_back(socket);

    //Successfully incoming connection
    qDebug() << "SRVER::Clinet connected " << socketDescriptor << '\n';

}

//Slot for reading messages
void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();

    //QDataStream for reading data
    QDataStream inDataStream(socket);

    //QDataStream version
    inDataStream.setVersion(QDataStream::Version::Qt_6_3);

    if(inDataStream.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(this->_blockSize == 0)
            {
                 qDebug() << "SERVER::Data block size = 0";    //Debug info

                if(this->socket->bytesAvailable() < 2) //If no byte available quit
                {
                    qDebug() << "SERVER::Socket bytes available < 2";    //Debug info
                    break;
                }

                //Reading size of data block
                inDataStream >> this->_blockSize;
            }


            if(this->socket->bytesAvailable() < this->_blockSize) //Data sending is not complete
            {
                qDebug() << "SERVER::Not full data";    //Debug info
                break;
            }

            QString user_name;
            QString message;
            QTime   time;

            //Read message from data stream
            inDataStream >> time >>user_name >> message;

            this->_blockSize = 0;

            this->sendToClient(message, user_name);

            //break;
        }
    }
    else
    {
        qDebug() << "SERVER::DataStream reading error \n";
    }
}

//Sending message to client
void Server::sendToClient(QString message,  QString user_name)
{
    //Clearing data before sending new message
    this->_data.clear();

    //DataStream for sending data
     QDataStream outDataStream(&this->_data, QIODevice::WriteOnly);

     //QDataStream version
     outDataStream.setVersion(QDataStream::Version::Qt_6_3);

     outDataStream << quint16(0) << QTime::currentTime() << user_name << message;

     outDataStream.device()->seek(0);
     outDataStream << quint16(this->_data.size() - sizeof(quint16));

     //Writing data bytes in sockets
     for(auto& socket : this->_sockets)
     {
         socket->write(this->_data);
     }
}



