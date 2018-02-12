#ifndef GLOBALVAR_H
#define GLOBALVAR_H
#include<QString>
extern char * uname;
extern char * passwd;
extern  QString  * LocalPWD;//本地工作目录
extern QString * RemotePWD;//远程工作目录
void initGlobal();

#endif // GLOBALVAR_H
