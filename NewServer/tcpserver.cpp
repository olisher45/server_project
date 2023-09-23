#include "tcpserver.h"
#include "server.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection,
            this, &TcpServer::slotNewConnection);

    if(!tcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "Сервер не запущен";
    } else {
        serverStatus = 1;
        qDebug() << "Сервер запущен";
    }
}

void TcpServer::slotNewConnection()
{
    QTcpSocket* socket = tcpServer->nextPendingConnection();
    if(serverStatus==1){
        socket->write("Добро пожаловать на сервер!\r\n");
        connect(socket, &QTcpSocket::readyRead,
                this,&TcpServer::slotServerRead);
        connect(socket,&QTcpSocket::disconnected,
                this,&TcpServer::slotClientDisconnected);
    }
    tcpSocket.insert(socket->socketDescriptor(), socket);
}

TcpServer* TcpServer::getInstance()
{
    if (!p_instance)
        p_instance = new TcpServer();
    return p_instance;
}

void TcpServer::slotClientDisconnected()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->close();
}

void TcpServer::slotServerRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    QString data;
    QByteArray response;
    while(socket->bytesAvailable()>0) {
        data.append(socket->readLine());
    }
    if (data.size() > 0 && data.back() == '\n') {
        QStringList datas = data.split("\n");
        datas.removeLast();
        for (int i = 0; i < datas.size(); i++) {
            response = Server::getInstance()->parse(data);
            socket->write(response + '\n');
            data.clear();
        }
    }
}

