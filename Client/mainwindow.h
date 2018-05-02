#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QObject>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void readyRead();

    void on_loginButton_clicked();

    void on_sendButton_clicked();
    void  disconnected();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QJsonDocument doc;
    QJsonParseError docError;
    static bool isConnected;
};

#endif // MAINWINDOW_H
