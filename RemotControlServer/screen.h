#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QPixmap>
#include <QTcpSocket>
#include <QFile>
#include <QThread>
#include <windows.h>
#include <QMouseEvent>

namespace Ui {
	class Screen;
}

class Screen : public QWidget
{
	Q_OBJECT
	
public:
	explicit Screen(QWidget *parent = 0);
	~Screen();
	typedef struct 
	{
	     int len;    // jpg文件大小
	} ScreenSpyHeader;
	
signals:
	void sendGetListFile(QString path);
	void dissConnectOpenFile(Screen *);		//此信号告诉MainWIndow已经绑定socket了，不需要再绑定了

public slots:
	void processBuffer();
	void setSocket(QTcpSocket* socket,int id = 0);
	void leave();
	void resizeEvent(QResizeEvent *);
	
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	
private:
	Ui::Screen *ui;
	QTcpSocket *socket;
	QTcpSocket *mouseSocket = NULL;
	
	QPixmap mScreenPixmap;    // 当前屏幕截图
//	unsigned int mScreenLen =0;    // JPG图片大小
	int overReadSize =0;
	int revSize =0;
	QByteArray img;
};

#endif // SCREEN_H
