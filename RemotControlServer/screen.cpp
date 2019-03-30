#include "screen.h"
#include "ui_screen.h"
#include <qDebug>
#include <QPicture>

Screen::Screen(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Screen)
{
	ui->setupUi(this);
}

Screen::~Screen()
{
	qDebug()<<"FILE 析构函数";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	delete ui;
}

void Screen::processBuffer()
{
	LARGE_INTEGER litmp;  
	LONGLONG Qpart1,Qpart2,Useingtime;  
	double dfMinus,dfFreq,dfTime;  
	  
	//获得CPU计时器的时钟频率  
	QueryPerformanceFrequency(&litmp);//取得高精度运行计数器的频率f,单位是每秒多少次（n/s），  
	dfFreq = (double)litmp.QuadPart;  
	  
	QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值  
	Qpart1 = litmp.QuadPart; //开始计时  
	
	
	if(revSize == 0)
		{
			char buf[100];
			QByteArray array = this->socket->readAll();
			memcpy(buf,array.data(),99);
			
			char *tok = strtok(buf, "#");	
			int size = atoi(tok);
			overReadSize = size;
			array.remove(0,strlen(tok)+1);
			revSize += array.size();
			qDebug()<<"array size:"<<array.size();
			if(size == array.size() || revSize +100 >= overReadSize)	//误差大小100b
				{
					if(!mScreenPixmap.loadFromData(array))
						{
							qDebug()<<"抛弃";
							return;
						}
					
					ui->label->setPixmap(mScreenPixmap);
					img.clear();
					revSize = 0;
					overReadSize = 0;
					
					QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值  
					Qpart2 = litmp.QuadPart; //终止计时 
					dfMinus = (double)(Qpart2 - Qpart1);//计算计数器值  
					dfTime = dfMinus / dfFreq;//获得对应时间，单位为秒,可以乘1000000精确到微秒级（us）
					
					qDebug()<<"time end:"<<dfTime*1000;//计算计数器值  
					return;
				}
			else
				{
					img = array;
				}
		}
	else
		{
			QByteArray array = this->socket->readAll();
			revSize += array.size();
			qDebug()<<"array size:"<<array.size();
			img += array;
			if(revSize == overReadSize || revSize +100 >= overReadSize)	 //误差大小100b
				{

					if(!mScreenPixmap.loadFromData(img))
						{
							qDebug()<<"抛弃";
							return;
						}
					
					ui->label->setPixmap(mScreenPixmap);
					img.clear();
					revSize = 0;
					overReadSize = 0;
					
					QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值  
					Qpart2 = litmp.QuadPart; //终止计时  
					
					dfMinus = (double)(Qpart2 - Qpart1);//计算计数器值  
					dfTime = dfMinus / dfFreq;//获得对应时间，单位为秒,可以乘1000000精确到微秒级（us）
					qDebug()<<"time end:"<<dfTime*1000;//计算计数器值  
					return;
				}
		}
	
}

void Screen::setSocket(QTcpSocket *sock, int id)
{
	qDebug()<<"正在设置屏幕端的socket";
	this->socket = sock;
//	this->mouseSocket = mouse;
	
	connect(this->socket,&QTcpSocket::readyRead,this,&Screen::processBuffer);
	connect(this->socket,&QTcpSocket::disconnected,this,&Screen::leave);
//	connect(this->mouseSocket,&QTcpSocket::disconnected,this,&Screen::leave);
	
	emit dissConnectOpenFile(this);
}

void Screen::leave()			//断开连接后的处理函数
{
	qDebug()<<"Leave()";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	this->mouseSocket->disconnectFromHost();
	this->mouseSocket->deleteLater();
	this->close();
}

void Screen::resizeEvent(QResizeEvent *)
{
	// 重设mScreenLabel大小
	ui->label->setGeometry(0,0,width(), height());
}

void Screen::mouseMoveEvent(QMouseEvent *event)
{
	if(this->mouseSocket == NULL)
		{
			return;
		}
	QString head = "move#";
	QString xs = QString::number(event->x());
	QString ys = QString::number(event->y());
	head = head + xs+"#"+ys;
	qDebug()<<"move : "<<head;
	this->mouseSocket->write(head.toLatin1());
}

void Screen::mousePressEvent(QMouseEvent *event)
{
	if(this->mouseSocket == NULL)
		{
			return;
		}
	QString head = "press#";
	this->mouseSocket->write(head.toLatin1());
}

void Screen::mouseReleaseEvent(QMouseEvent *event)
{
	if(this->mouseSocket == NULL)
		{
			return;
		}
	QString head = "release#";
	this->mouseSocket->write(head.toLatin1());
//	qDebug()<<""
}


