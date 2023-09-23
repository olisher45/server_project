#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QCoreApplication>
#include <QString>

class TcpServer: public QObject
{
public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
private:
    static inline TcpServer *p_instance = nullptr;
    TcpServer(QObject *parent= nullptr);
    TcpServer(const TcpServer& );
    TcpServer& operator = (TcpServer &);

    QTcpServer* tcpServer;
    int serverStatus;
    QMap<int, QTcpSocket*> tcpSocket;
public:
    static TcpServer* getInstance();
};

#endif // TCPSERVER_H
