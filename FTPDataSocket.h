#ifndef FTPDATASOCKET_H
#define FTPDATASOCKET_H

#include <QObject>
#include<QTcpSocket>
typedef enum{
    DATARETR,
    DATASTOR,
    DATANLST
}workmode;
class FTPDataSocket : public QTcpSocket
{
    Q_OBJECT
private:
    workmode * mmode;//工作模式
    QString * mArgs;//执行的参数
    QString * NLSTbuff;
    bool isdisconnected;//已经断开
     qint64 * iseek;//接收数据偏移
signals:
    void dirList(QStringList list);//获取到了目录
    void DownloadStarting(QString file);//开始下载
    void DownloadSuccess(QString file);//下载成功
public:
    FTPDataSocket(QString ip,int port,workmode mode,QString args);
private slots:
    void conned();
    void recv();
    void setdisconned();
};

#endif // FTPDATASOCKET_H
