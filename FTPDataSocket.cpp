#include "FTPDataSocket.h"
#include<QDebug>
#include<QFile>
#include<QHostAddress>
FTPDataSocket::FTPDataSocket(QString ip,int port,workmode mode,QString  args):QTcpSocket(nullptr),
    mmode(new workmode),
    mArgs(new QString),
    NLSTbuff(new QString),
    iseek(new qint64)

{
    *mmode=mode;
    *mArgs=args;
     this->isdisconnected=false;
     *iseek=0;
    if(mode==DATARETR)
    {
        QFile f("D://"+args);
        if(f.exists()==true)f.remove();
    }
    QObject::connect(this,SIGNAL(connected()),this,SLOT(conned()));
    QObject::connect(this,SIGNAL(readyRead()),this,SLOT(recv()));
    QObject::connect(this,SIGNAL(disconnected()),this,SLOT(setdisconned()));
    this->connectToHost(QHostAddress(ip),port);


}
void  FTPDataSocket::conned()
{
    qDebug()<<"数据连接连接服务器成功";

    if(*mmode==DATASTOR)
    {
        char data[1];
        int readamount=0;
        QFile f("D://"+*mArgs);
        f.open(QIODevice::ReadOnly);
        while((readamount=f.read(data,1))>0)
        {
            this->write(data,1);
        }
        this->close();//写完就立即关闭套接字
    }
}
void FTPDataSocket::recv()
{
   // qDebug()<<"接收";
    char x[1024];
    int amount=0;
    ///
    if(*(this->mmode)==DATARETR)//接收文件
    {
        QString file= QString("D://")+QString(*mArgs);
        QFile f(file);
       f.open(QIODevice::Append);
        while((amount=this->read(x,1024))!=0)
        {
           f.write(x,amount);
           //*iseek=*iseek+amount;
           //qDebug()<<*x;
        }
        f.close();
    }
    else if(*(this->mmode)==DATANLST)//接收目录信息 \r\n隔开
    {
        //NLSTbuff
        while((amount=this->read(x,1023))>0)
        {
           *(x+amount+1)='\0';//char *
           (*NLSTbuff).append(x);

        }

        qDebug()<<*NLSTbuff;
    }


}
void FTPDataSocket::setdisconned()
{
    this->isdisconnected=true;
    if(*(this->mmode)==DATANLST)
    {
        //如果已经断开了
        QStringList dirlist= NLSTbuff->split("\r\n");
        for(int i=0;i<dirlist.length();i++)
        {
           qDebug()<<dirlist.at(i);
        }
        //这里还有一个小bug 最后一个 split出来的字符串为空
        emit  dirList(dirlist);//此类的信号  发给 FTPCommander接收 ，再传给UI
    }

}
