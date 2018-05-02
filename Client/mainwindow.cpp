#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QNetworkInterface>
#include <QList>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()),
            this,   SLOT(readyRead()));
    ui->roomsTextEdit->setReadOnly(true);
    ui->usersTextEdit->setReadOnly(true);
    this->setWindowTitle("Chat");
    connect(socket, SIGNAL(disconnected()),
            this,   SLOT(disconnected()));
    connect(ui->lineEdit, SIGNAL(returnPressed()),
            ui->sendButton, SLOT(click()));


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::readyRead()
{

   while(socket->canReadLine())
   {
       QString line = QString::fromUtf8(socket->readLine().trimmed());
       doc = QJsonDocument::fromJson(line.toUtf8(), &docError);
       if (docError.errorString().toInt() == QJsonParseError::NoError){
           if(doc.object().value("type").toString() == "message_from_user"){
               QString user = doc.object().value("user_name").toString();
               QString message = doc.object().value("message").toString();
               ui->roomsTextEdit->append("<b>" + user +"</b>: " + message);
           }
           else if(doc.object().value("type").toString() == "wrong_name"){
               QMessageBox::information(this, "Error", "That name is already taken");
               ui->loginButton->setText("Connect");
               isConnected = false;
               ui->roomsTextEdit->append("You are disconnected from server");
               ui->loginLineEdit->setDisabled(false);
               ui->usersTextEdit->clear();
           }
           else if(doc.object().value("type").toString() == "message_from_server"){
               QString message = doc.object().value("message").toString();
               ui->roomsTextEdit->append("<b>Server</b>: " + message);
           }
           else if(doc.object().value("type").toString() == "users_list"){
               QStringList users = doc.object().value("users").toString().split(",");
               ui->usersTextEdit->clear();

               foreach(QString user, users)
                 ui->usersTextEdit->append(user);

           }
       }

   }

}


bool MainWindow::isConnected = false;

void MainWindow::on_loginButton_clicked()
{
    if(ui->loginLineEdit->text().isEmpty()){
        QMessageBox::information(this, "Error", "Please, enter your name correctly");

    }
    else { 

        if(!isConnected){
            socket->connectToHost("192.168.3.100", 56343);
            if(socket->waitForConnected(200))
            {
                isConnected = true;
                ui->loginButton->setText("Disconnect");
                socket->write(QString("{\"type\":\"user_name\",\"user_name\":\"" + ui->loginLineEdit->text() + "\"}\n").toUtf8());
                ui->loginLineEdit->setDisabled(true);
            }
            else{
                QMessageBox::information(this, "Error", "Failed to connect to server");
            }
        }
        else{
            socket->disconnectFromHost();
        }

    }
}
void MainWindow::on_sendButton_clicked()
{
    if(socket->waitForConnected(200)){
        QString message = ui->lineEdit->text().trimmed();
        if(!message.isEmpty())
        {
            socket->write(QString(message + "\"}\n").toUtf8());
        }
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
    }
    else{
        QMessageBox::information(this, "Error", "Connetion failed");

    }
}

void MainWindow::disconnected()
{
    socket->close();
    ui->loginButton->setText("Connect");
    isConnected = false;
    ui->roomsTextEdit->append("You are disconnected from server");
    ui->loginLineEdit->setDisabled(false);
    ui->usersTextEdit->clear();
}


