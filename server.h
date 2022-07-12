#ifndef SERVER_H
#define SERVER_H

#include<mutex>
#include<condition_variable>
#include<thread>
#include<QTcpServer>
#include<QTcpSocket>
#include<QVector>
#include<QTime>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(const int& sever_port);
    ~Server();

    QTcpSocket* socket;

    const int& getPort() const;

private:
    //Port info
    const int _port;

    //Sockets
    QVector<QTcpSocket*> _sockets;

    //Byte array data
    QByteArray _data;

    //Size of data block
    quint16 _blockSize;

    void sendToClient(QString message, QString user_name);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();

};

#endif // SERVER_H
