#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>
#include "thread.h"
struct socktlib		//存放socket
{
	Thread *thread;
	QTcpSocket *socket;
	int id;
};
class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = nullptr);
	QList<socktlib> List;
signals:
	void sendLoginInfo(int id , QString ip,QString name,QString systeminfo);	//中间信号 传递登录信息到界面
	void sendLogoutInfo(int id);		//中间信号 传递退出信息到界面
	void sendOpenFile(QTcpSocket *socket,int id =0);
public slots:
	void incomingConnection(int id); //新连接处理
	
	bool startServer(int port);
	void stopServer();
	
private slots:
	void getLoginInfo(int id , QString ip,QString name,QString systeminfo);	//中间函数 传递登录信息到界面
	void getLogoutInfo(int id);
	void getOpenFile(QTcpSocket *socket,int id =0);
	void deleteList(int id);
};

#endif // SERVER_H
