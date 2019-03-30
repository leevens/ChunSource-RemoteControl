#include "keyboard.h"
#include "ui_keyboard.h"

Keyboard::Keyboard(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Keyboard)
{
	ui->setupUi(this);
}

Keyboard::~Keyboard()
{
	delete ui;
}

void Keyboard::setSocket(QTcpSocket *socket, int id)
{
	qDebug()<<"正在设置文件端的socket";
	this->socket = socket;
	connect(this->socket,&QTcpSocket::readyRead,this,&Keyboard::processReturn);
	connect(this->socket,&QTcpSocket::disconnected,this,&Keyboard::leave);
	emit dissConnectOpenFile(this);
}

void Keyboard::leave()
{
	qDebug()<<"Leave()";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	this->close();
}

void Keyboard::processReturn()
{
	QByteArray buf = this->socket->readAll();
	ui->EditKeyBoard->append(buf);
}
