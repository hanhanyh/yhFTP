#include "FTPCommander.h"
#include<QDebug>
#include<stdlib.h>
#include<QRegExp>
#include<QThread>
#include"FTPDataSocket.h"
#include"globalvar.h"
FTPCommander::FTPCommander(QObject * parent):QTcpSocket(parent),ftpstatus(new ftpStat),
   sendbuff(new char[1024]),
    recvbuff(new char[1024]),
    willDataStatus(new ftpStat),
    willDataArgs(new QString)
 //   username(new char[1024]),
 //   userpwd(new char[1024])//Uname全部放入全局变量
{

    *willDataStatus=NOOP;//初始化
     connect(this,SIGNAL(connected()),this,SLOT(conned()));//成功连接服务器
     connect(this,SIGNAL(readyRead()),this,SLOT(recvComm()));
     connect(this,SIGNAL(NeeedUserName()),this,SLOT(Login()));
}
FTPCommander:: ~FTPCommander()
{
    delete ftpstatus;
    delete [] sendbuff;
    delete  [] recvbuff;
    delete willDataStatus;
    delete [] willDataArgs;
  //  delete username;
  //  delete userpwd;
}
void FTPCommander:: conned()
{
    qDebug()<<"连接服务器成功"<<endl;
    *ftpstatus=WELCOME;
}

void FTPCommander:: recvComm()
{
    switch(*ftpstatus)
    {

        case WELCOME://连接服务器成果后显示欢迎信息
             {
                    this->read(recvbuff,1024);//欢迎信息有很多行
                    qDebug()<<recvbuff;
                    //显示欢迎过后设置用户名和密码
                    qDebug()<<"状态码："<<getServerStatus();//220欢迎
                    if(getServerStatus()==220)
                    {
                        emit NeeedUserName();
                    }
                 // this->setUser("hanhan");
            }
        break;
        case USER:
            {
               // this->read(recvbuff,1024);
              int amou= this->readLine();
               qDebug()<<recvbuff;
                 this->setPass();
             //  qDebug()<<"状态码："<<getServerStatus();//231需要密码
               if(getServerStatus()==231)
               {
                   emit UserSended();
               }
            }
        break;
        case PASS:
          {
                //  this->read(recvbuff,1024);
               int amou= this->readLine();
               qDebug()<<amou;
               qDebug()<<recvbuff;
                 // qDebug()<<"状态码："<<getServerStatus();//230登录成功
                 if(getServerStatus()==230)//判断状态码 发送信号(从recv缓存中直接获取状态码)
                  {
                      emit LoginSuccess();//登录成功
                  }
                  else
                  {
                      emit LoginFailure();//登录失败

                  }
                  getPwd();
            }

        break;
        case PASV://先设置PASV状态 再进行相关操作比如列表  下载等等（这里是PASV ACK）
            {
                this->read(recvbuff,1024);
           // qDebug()<<"状态码："<<getServerStatus();//227 被动模式PASV
              qDebug()<<recvbuff;
              int a1,a2,a3,a4,a5,a6;
               char str[1024];
               sscanf(recvbuff,"%*[^(]%[^)]",str);
               sscanf(str,"(%d,%d,%d,%d,%d,%d)",&a1,&a2,&a3,&a4,&a5,&a6);
               printf("%d %d",a5*256,a6);\
               //a5*256+a6
               if(*willDataStatus==RETR)//下载
               {
                   *ftpstatus=RETR;
                   memset(sendbuff,'\0',1024);
                   QString willdata;
                   willdata=QString("RETR ")+willDataArgs+"\r\n";
                 //  sprintf(sendbuff,"%s %s\r\n","RETR",willDataArgs);
                   this->write(QByteArray::fromStdString(willdata.toStdString()));
                   this->flush();
                   startDataThread("127.0.0.1",a5*256+a6,DATARETR);
               }else if(*willDataStatus==NLST)
               {
                   *ftpstatus=NLST;
                   memset(sendbuff,'\0',1024);
                   sprintf(sendbuff,"%s \r\n","NLST");//命令 空格 回车换行
                   this->write(sendbuff,1024);
                   this->flush();
                   startDataThread("127.0.0.1",a5*256+a6,DATANLST);//获取列表模式
               }
            ///test

            ///
            }

        break;

    case CWD:
        {}
        break;
    case PWDD://PWD获取服务器当前工作目录
          {
                memset(recvbuff,'\0',1024);
               this->read(recvbuff,1023);
                QString reBuff(recvbuff);
                QString sstatus=reBuff.mid(0,3);
                if(sstatus.toInt()==257)
                {
                    qDebug()<<257;
                    QRegExp exp("\"(.*)\"");
                    exp.indexIn(reBuff);
                    QString cap1=  exp.cap(0);//cap1是包括了 表达式的 ""
                    QString cap2=exp.cap(1);//cap2就是 没有包括""直接取里面的内容
                    qDebug()<<cap2;
                    *RemotePWD=cap2;//复制全局变量
                    emit PWD(cap2);
                }
           }
        break;
    case RETR://下载
        {
            int currStatus= this->getServerStatus();
            if(currStatus==226)//下载成功
            {
                emit DownLoadSuccess();
            }
        }
        break;
    case STOR:{}
        break;
    case SIZE:{}
        break;
    case NLST:{}
        break;
    case QUIT:{}
        break;
    case NOOP:{}
        break;
    default:{}
        break;

    }
}
//设置用户名
void  FTPCommander::setUser()
{
    *ftpstatus=USER;
    memset(sendbuff,'\0',1024);
    sprintf(sendbuff,"USER %s\r\n",uname);  //命令[一个空格]参数\r\n
    this->write(sendbuff,strlen(sendbuff));//注意 sizeof 和 strlen区别
    this->flush();
   // qDebug()<<i;
}
//设置密码
void FTPCommander::setPass()
{
     *ftpstatus=PASS;
    memset(sendbuff,'\0',1024);
    sprintf(sendbuff,"PASS %s\r\n",passwd);
    this->write(sendbuff,strlen(sendbuff));
    this->flush();
}
//设置PASV模式
void FTPCommander::setPASV()
{
    *ftpstatus=PASV;
   memset(sendbuff,'\0',1024);
   sprintf(sendbuff,"%s \r\n","PASV");
   this->write(sendbuff,strlen(sendbuff));
   this->flush();
}
//根据服务器返回字符串 获得状态码
int FTPCommander::getServerStatus()
{
    int statucode;
    char data[1024];
    sscanf(recvbuff,"%3s",data);//这里可以用正则 可以用这样获取前三位
     statucode= atoi(data);//ASCII to Int
    return statucode;
}
//登录
void FTPCommander::Login()
{
    //这里需要以后设置全局变量 放里面
    // char uname[]="anonymous";
  //   char userpwd[]="anonymous";
  //  *(this->username)=*uname;
  //   *(this->userpwd)=*userpwd;
  //  sprintf(this->username,"%s",uname);
  //   sprintf(this->userpwd,"%s",userpwd);
     setUser();//设置用户名（发送用户名）
     connect(this,SIGNAL(UserSended()),this,SLOT(setPass()));//发送完用户名后发送密码
}
void FTPCommander::Retr(QString arg)//下载
{

    *willDataStatus=RETR;
    *willDataArgs=arg;
    setPASV();//设置被动模式 在 收到 被动模式的通知ACK后去判断willData 再发送数据包

}
void FTPCommander::Nlst()//显示列表
{
    *willDataStatus=NLST;
    *willDataArgs="NLST \r\n";
    setPASV();//设置被动模式在收到服务器数据传输端口后 去判断WillDataStaus进行发包

}
void FTPCommander::getPwd()//显示工作目录
{
    //257 "/" is current directory.
    *ftpstatus=PWDD;
   // *sendbuff="PWD \r\n";
    memset(sendbuff,'\0',1024);
    strcpy(sendbuff,"PWD \r\n");
    this->write(sendbuff,strlen(sendbuff));//显示当前工作目录
}
//开启数据传输线程
 void FTPCommander::startDataThread(QString ip,int port,workmode mode)//开启数据线程
{
     QThread *thread=new QThread();
     FTPDataSocket* socket=new FTPDataSocket(ip,port,mode,*willDataArgs);//这里的参数一般是 文件名 用来保存
     socket->moveToThread(thread);
     //connect(socket,SIGNAL(disconnected()),thread,SLOT(quit()));
     //connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
     //connect(socket,SIGNAL(disconnected()),thread,SLOT(deleteLater()));
      thread->start();

}
void FTPCommander::setCWD(QString path)//改变当前工作目录
{
     //250 CWD successful. "/360Downloads " is current directory.
}
//读取一行（\r\n回车换行）到接受缓冲区
int  FTPCommander::readLine()
{
    //可以参照  unix 的readn 和writen的实现
    int amount=0;//当前读入数量
    int offset=0;//偏移
    int maxsize=1023;//缓冲区大小
    memset(recvbuff,'\0',1024);
    //每次只读一个字节用来判断\r\n
    while((amount=this->read(recvbuff+offset,1))>0 && maxsize-offset>0)
    {
        offset+=amount;//设置偏移
        if(*(recvbuff+offset-amount)=='\r')
        {
            if((amount=this->read(recvbuff+offset,1))>0&& maxsize-offset>0)
            {
                 offset+=amount;//设置偏移
                 if(*(recvbuff+offset-amount)=='\n')
                 {
                     return offset;
                 }
            }else{
                return -1;//没有读取到回车换行
            }
        }
    }
    return -1;//没有读取到回车换行
}
