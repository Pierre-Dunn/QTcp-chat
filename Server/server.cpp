#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent){}


void Server::startServer()
{
    if(this->listen(QHostAddress::Any, 6212))
            qDebug() << "Listening";
    else
            qDebug() << "isnt listening";
}
void Server::incomingConnection(int socketDescriptor)
{
    clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);
    clients.insert(clientSocket);

    qDebug() <<"New client has joined to chat from: "
             << clientSocket->peerAddress().toString();

    connect(clientSocket, SIGNAL(readyRead()),
            this,         SLOT(readyRead()));
    connect(clientSocket, SIGNAL(disconnected()),
            this,         SLOT(disconnected()));
}

bool Server::uniqName(QString name)
{
    foreach(QTcpSocket *client, clients)
        if(users[client] == name)
            return false;
    return true;
}

void Server::readyRead()
{    
    clientSocket = (QTcpSocket*)sender();
    while(clientSocket->canReadLine()){
        QString line = QString::fromUtf8(clientSocket->readLine());    
        doc = QJsonDocument::fromJson(line.toUtf8(), &docError);
        qDebug() << "Read line: " << line;
        if (docError.errorString().toInt() == QJsonParseError::NoError){

            if (doc.object().value("type").toString() == "user_name")
            {
                if(!uniqName(doc.object().value("user_name").toString())) //Если уже есть юзер с таким именем
                {
                     qDebug() << "wrong username: is already exist";
                     clientSocket->write(QString("{\"type\":\"wrong_name\"}\n").toUtf8());
                     clients.remove(clientSocket);
                     return;
                }
                QString user = line;
                users[clientSocket] = doc.object().value("user_name").toString();
                foreach(QTcpSocket *client, clients)
                {
                    if(client == clientSocket)
                        client->write(QString("{\"type\":\"message_from_server\",\"message\":\"Welcome to chat, " + doc.object().value("user_name").toString() + "!\"}\n").toUtf8());
                    else
                        client->write(QString("{\"type\":\"message_from_server\",\"message\":\"" + doc.object().value("user_name").toString() + " has joined.\"}\n").toUtf8());
                }
                sendUsersList();
                qDebug() << doc.object().value("user_name").toString() << " connected";
            }
            else if(users.contains(clientSocket))
            {
                QString message = line;
                QString user = users[clientSocket];
                qDebug() << user << " send: " << line;
                foreach(QTcpSocket *client, clients)
                    client->write(QString("{\"type\":\"message_from_user\",\"user_name\":\"" + user +"\",\"message\":\"" + message).toUtf8());
            }
            else
            {
                qWarning() << "Got bad message from client:" << clientSocket->peerAddress().toString() << line;
            }

        }

    }
}

void Server::disconnected()
{
    clientSocket = (QTcpSocket*)sender();
    if(clients.contains(clientSocket))
    {
        qDebug() << users[clientSocket] << " disconnected";
        clientSocket->write(QString("{\"type\":\"message_from_server\",\"message\":\"You are disconnected from server\"}\n").toUtf8());
        clients.remove(clientSocket);

        QString user = users[clientSocket];
        users.remove(clientSocket);

        sendUsersList();
        foreach(QTcpSocket *client, clients)
            client->write(QString("{\"type\":\"message_from_server\",\"message\":\"" + user + " has left.\"}\n").toUtf8());
    }
}

void Server::sendUsersList()
{
    QStringList list;
    foreach(QString user, users.values())
        list << user;
    foreach(QTcpSocket *client, clients)
        client->write(QString("{\"type\":\"users_list\",\"users\":\"" + list.join(",") + "\"}\n").toUtf8());
}
