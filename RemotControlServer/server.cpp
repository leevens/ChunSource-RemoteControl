#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{
	
}

void Server::incomingConnection(int id)
{
	qDebug()<<"\nnewconnect id :"<<id;
	socktlib lib;
	
	Thread *t = new Thread(id);
	connect(t,&Thread::sendLoginInfo,this,&Server::getLoginInfo);
	connect(t,&Thread::sendLogoutInfo,this,&Server::getLogoutInfo);
	connect(t,&Thread::sendOpenFile,this,&Server::getOpenFile);
	connect(t,&Thread::deleteFile,this,&Server::deleteList);
	t->run();
	
	lib.id = id;
	lib.socket = t->MySocket;
	lib.thread = t;
	List << lib;
}

bool Server::startServer(int port)
{
	if(this->listen(QHostAddress::Any,port))
		{
			qDebug()<<"listen:"<<port;
			return true;
		}
	else
		{
			return false;
		}
}

void Server::stopServer()
{
	int size = List.size();
	for(int i=0;i<size;i++)
		{
			
			socktlib lib = List.at(i);
			lib.socket->disconnectFromHost();
			lib.socket->deleteLater();
			lib.thread->deleteLater();
			qDebug()<<"delete : "<<lib.id;
		}
	
	List.clear();
	
	if(this->isListening())
		{
			this->close();
		}
}

void Server::getLoginInfo(int id, QString ip, QString name, QString systeminfo)
{
	emit sendLoginInfo(id,ip,name,systeminfo);
}

void Server::getLogoutInfo(int id)
{
	emit sendLogoutInfo(id);
	for(int i =0; i<List.size();i++)
		{
			socktlib tlib = List.at(i);
			if(tlib.id == id)
				{
					List.removeAt(i);
				}
		}
}

void Server::getOpenFile(QTcpSocket *socket,int id)
{
	if(id == 1)
		{
			emit sendOpenFile(socket,1);
			return;
		}
	emit sendOpenFile(socket);
}

void Server::deleteList(int id)
{
	for(int i =0; i<List.size();i++)
		{
			socktlib tlib = List.at(i);
			if(tlib.id == id)
				{
					List.removeAt(i);
				}
		}
	qDebug()<<"delete sonServer ID";
}
