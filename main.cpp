#include <QCoreApplication>
#include<QTcpSocket>
#include<QHostAddress>
#include"FTPCommander.h"
#include"globalvar.h"
QTcpSocket * socket;
 FTPCommander * ftp;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    initGlobal();
    ftp =new FTPCommander();
    ftp->connectToHost(QHostAddress("127.0.0.1"),21);
    return a.exec();
}
