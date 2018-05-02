#ifndef SERVER_H
#define SERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QObject>
class Server : public QTcpServer
{
    Q_OBJECT
private:
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*,QString> users;
    QJsonDocument doc;
    QJsonParseError docError;
    QTcpServer *server;
    QTcpSocket *clientSocket;
    QByteArray data;
public:
    Server(QObject *parent = 0);
    bool uniqName(QString name);
public slots:
    void startServer();
    void readyRead();
    void disconnected();
    void sendUsersList();
    void incomingConnection(int socketDescriptor);
};

#endif // SERVER_H
