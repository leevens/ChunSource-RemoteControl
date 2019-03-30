#include "thread.h"
#include <QDebug>

Thread::Thread(int id,QObject *parent) : QObject(parent)
{
	this->SocketID = id;
	qDebug()<<"new thread";
}

void Thread::run()
{
	QTcpSocket *socket = new QTcpSocket();
	this->MySocket = socket;
	if(!socket->setSocketDescriptor(this->SocketID))
		{
			qDebug()<<"Error set socketID:"<<socket->error();
		}
	qDebug()<<"####this socket:"<<socket->socketDescriptor();
	
	connect(socket,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(socket,&QTcpSocket::readyRead,this,&Thread::read,Qt::DirectConnection);
	//	connect(this,&Thread::sendData,this,&Thread::write,Qt::QueuedConnection);
	QTimer *time = new QTimer();
	this->MyTime = time;
	time->start(5000);
}

void Thread::shutdown()
{
	this->MySocket->disconnectFromHost();
	this->MySocket->deleteLater();
}

void Thread::write(QString buf)
{
	this->MySocket->write(buf.toLatin1());
}

void Thread::timeOuter()
{
	this->MySocket->disconnectFromHost();
	this->MySocket->deleteLater();
	this->MyTime->stop();
	this->MyTime->deleteLater();
	//	this->quit();
}

void Thread::read()
{
	char myCharbuff[4096];
	this->MySocket->read(myCharbuff,4096);
	
	
	wchar_t *t = new wchar_t[4096];
	t = char2wchar(myCharbuff);
	
	QString buf;
	buf = QString::fromWCharArray(t);
	
	
	qDebug()<<"read  :"<<buf;
	buf = buf.section("&",0,0);
	
	qDebug()<<"buf:\n/********************/\n"<<buf<<"\n/***************************/\n";
	if(this->SocketNumber ==0 && buf.indexOf("chunsource") >= 0)	//初始化通讯 "chunsource" 初始化协议头
		{
			this->MyTime->stop();
			this->MyTime->deleteLater();
			this->SocketNumber =1;
			this->MySocket->write("chunsource&");
			return;
		}
	else if(this->SocketNumber ==0 && buf.indexOf("SCREEN") >= 0)
		{
			qDebug()<<"SCREEN ";
			emit sendOpenFile(this->MySocket);
			this->MySocket->write("OPEN_SCREEN&");
			disconnect(this->MySocket,&QTcpSocket::disconnected,this,&Thread::leave);
			disconnect(this->MySocket,&QTcpSocket::readyRead,this,&Thread::read);
			emit deleteFile(this->SocketID);
			this->deleteLater();
		}
	else if(this->SocketNumber ==0 && buf.indexOf("MOUSE") >= 0)
		{
			qDebug()<<"MOUSE ";
			emit sendOpenFile(this->MySocket,1);
			this->MySocket->write("OPEN_MOUSE&");
			disconnect(this->MySocket,&QTcpSocket::disconnected,this,&Thread::leave);
			disconnect(this->MySocket,&QTcpSocket::readyRead,this,&Thread::read);
			emit deleteFile(this->SocketID);
			this->deleteLater();
		}
	else if(this->SocketNumber ==0 && buf.indexOf("FILE") >= 0)
		{
			qDebug()<<" FILE CMD";
			emit sendOpenFile(this->MySocket);
			this->MySocket->write("OPEN_FILE&");
			disconnect(this->MySocket,&QTcpSocket::disconnected,this,&Thread::leave);
			disconnect(this->MySocket,&QTcpSocket::readyRead,this,&Thread::read);
			emit deleteFile(this->SocketID);
			this->deleteLater();
			//	qDebug()<<"delete fileID";
		}
	else if(this->SocketNumber ==0 && buf.indexOf("KEYBOARD") >= 0)
		{
			qDebug()<<" FILE CMD";
			emit sendOpenFile(this->MySocket);
			this->MySocket->write("OPEN_KEYBOARD&");
			disconnect(this->MySocket,&QTcpSocket::disconnected,this,&Thread::leave);
			disconnect(this->MySocket,&QTcpSocket::readyRead,this,&Thread::read);
			emit deleteFile(this->SocketID);
			this->deleteLater();
			//	qDebug()<<"delete fileID";
		}
	else if(this->SocketNumber == 0){	//(SocketNumber < 1)不为协议设备，关闭计时并断开连接
			this->MyTime->stop();
			this->MySocket->disconnectFromHost();
			this->MySocket->deleteLater();
			this->MyTime->deleteLater();
			//			this->exit();
			return;
		}
	
	if(this->SocketNumber ==1)
		{
			qDebug()<<"\nSocketNumber:"<<this->SocketNumber;
			QStringList list = buf.split("#");
			
			qDebug()<<"\nlist size:"<<list.size();
			qDebug()<<"list:"<<list;
			if(list.size() < 2)
				{		//登录信息不符合数量 name system
					this->MySocket->disconnectFromHost();
					this->MySocket->deleteLater();
					//				this->exit();
					return;
				}
			emit sendLoginInfo(this->SocketID,this->MySocket->peerAddress().toString(),list.at(0),list.at(1));
			this->SocketNumber =2;
		}
	
}

void Thread::leave()
{
	qDebug()<<this->SocketID<<" is quit";
	emit sendLogoutInfo(this->SocketID);
	this->MySocket->deleteLater();
	//	this->exit();
}

char *Thread::wchar2char(const wchar_t *wchar)
{
	
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

wchar_t *Thread::char2wchar(const char *cchar)
{
	
	wchar_t *m_wchar;    
	int len = MultiByteToWideChar( CP_ACP ,0,cchar ,strlen( cchar), NULL,0);     
	m_wchar= new wchar_t[len+1];     
	MultiByteToWideChar( CP_ACP ,0,cchar,strlen( cchar),m_wchar,len);     
	m_wchar[len]= '\0' ;    
	return m_wchar;
	
}
