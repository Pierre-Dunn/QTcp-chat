#include <QCoreApplication>
#include <server.h>
#include <windows.h>
int main(int argc, char *argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    QCoreApplication a(argc, argv);
    Server server;
    server.startServer();
   // return a.exec();
    return 0;
}
