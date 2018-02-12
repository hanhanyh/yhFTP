#include<stdlib.h>
#include<string.h>
#include"globalvar.h"
#include<QString>
char * uname=new char[1024];
char * passwd=new char[1024];
QString * LocalPWD=nullptr;
QString * RemotePWD=nullptr;
void initGlobal()
{
    strcpy(uname,"hanhan");
    strcpy(passwd,"hanhan");
    LocalPWD=new QString("D://");
    RemotePWD=new QString("/");
}
