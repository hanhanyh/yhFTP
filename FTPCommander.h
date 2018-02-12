#ifndef FTPCOMMANDER_H
#define FTPCOMMANDER_H
/*
   FTP命令发送端
*/
#include<QObject>
#include<QTcpSocket>
#include"FTPDataSocket.h"
typedef enum{
    WELCOME,   //连接服务器初始化状态
    USER,
    PASS,
    PASV,
    PWDD,//PWD
    CWD,
    RETR,
    STOR,
    SIZE,
    NLST,
    QUIT,
    NOOP  //什么都不做
}ftpStat;//当前所处状态
///
class FTPCommander:public QTcpSocket
{
    Q_OBJECT
signals:
    void LoginSuccess();//登录成功
    void LoginFailure();//登录失败
    void UserSended();//用户名发送成功
    void NeeedUserName();//接收欢饮信息需要发送用户名了
    void ConnectServerSuccess();//连接服务器成功
    void PWD(QString pwd);//当前工作目录
    void sCWD();//改变工作目录信号（成功）
    void DownLoadSuccess();//下载成功
private:
  //  char * username;//用户名和密码
//    char * userpwd;
    ftpStat * ftpstatus ;//当前正在执行的命令
    char * sendbuff;//缓冲（用于发送命令）
    char * recvbuff;//接收命令
    ftpStat * willDataStatus;//数据传输socket马上要执行的操作 如 RETR ,NLST
    QString  * willDataArgs;//数据传输socket的参数  如 RETR x.txt (x.txt)
    void setPASV();//设置PASV模式
    int getServerStatus();//根据服务器返回字符串获得状态码
    int readLine();//从套接字里面读取一行数据（\r\n）放入缓冲区（防止粘包）
private slots:
    void  setUser();//设置用户名(发送)
    void setPass();//和密码（发送）

public:
    FTPCommander(QObject * parent=nullptr);
    ~FTPCommander();
    void startDataThread(QString ip,int port,workmode mode);//开启数据线程

public slots:
    void conned();
    void recvComm();
     void Login();
     void Retr(QString arg);//下载
     void Nlst();//显示列表
     void getPwd();//显示工作目录
     void setCWD(QString path);//改变当前工作目录
};

#endif // FTPCOMMANDER_H
