#include "shell.h"
#include "ui_shell.h"

Shell::Shell(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Shell)
{
	ui->setupUi(this);
}

Shell::~Shell()
{
	delete ui;
}

void Shell::setSocket(QTcpSocket *socket, int id)
{
	qDebug()<<"正在设置文件端的socket"; 
	this->socket = socket;
	connect(this->socket,&QTcpSocket::readyRead,this,&Shell::processReturn);
	connect(this->socket,&QTcpSocket::disconnected,this,&Shell::leave);
	connect(ui->ButtonCheck,&QPushButton::clicked,this,&Shell::sendMessage);
	emit dissConnectOpenFile(this);
}

void Shell::leave()
{
	qDebug()<<"Leave()";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	this->close();
}

void Shell::processReturn()
{
	QByteArray buf = this->socket->readAll();
	ui->textEdit->append("\n"+buf);
}

void Shell::sendMessage()
{
	this->socket->write(ui->EditArg->text().toLatin1());
}
