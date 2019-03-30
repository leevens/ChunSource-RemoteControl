#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>
#include <windows.h>
#include "file.h"

class Thread : public QObject
{
	Q_OBJECT
public:
	explicit Thread(int id, QObject *parent = nullptr);
	QTcpSocket *MySocket;
	
	int SocketID;		//0:等待初始化  1:等待登录  2:等待命令回应
	
signals:
	void sendLoginInfo(int id , QString ip,QString name,QString systeminfo);
	void sendLogoutInfo(int id);
	void sendOpenFile(QTcpSocket *socket,int id =0);
	void sendData(QString buf);
	void deleteFile(int id);
private:
	QTimer* MyTime;
	int SocketNumber = 0;
public slots:
	void run();
	void shutdown();
	void write(QString buf);
static	char* wchar2char(const wchar_t * wchar);
static	wchar_t * char2wchar(const char* cchar);
private slots:
	void timeOuter();
	void read();
	void leave();

};

#endif // THREAD_H
