#ifndef FILE_H
#define FILE_H

#include <QWidget>
#include <QMainWindow>
#include <QPainter>
#include <QTableWidget>
#include <QFile>
#include <QDir>
#include <QTcpSocket>
#include <QFileInfo>
#include <iostream>
#include <QTextCodec>
#include <QString>
#include <QChar>
#include <QMessageBox>
#include <QMouseEvent>
#include "thread.h"
using namespace std;
namespace Ui {
	class File;
}
/************************
SEND_FILE
FILE_SIZE
*************************
SEND_FILELIST	
FILENAME
.....
*************************/
class File : public QWidget
{
	Q_OBJECT
	
public:
	explicit File(QWidget *parent = 0);
	~File();
	void processReturn();
	static void writeFile(QString path , QByteArray buf);	//写文件操作
	static void writeFile(QString path , char* buf);
	void setSocket(QTcpSocket* socket,int id =0);
private:
	Ui::File *ui;
	QString MyPath;		//当前显示的本地系统路径
	QString TargetPath;		//当前显示的目标系统路径
	QTcpSocket *socket;	
	bool CurrentStatus =false;	//判断是否为传输状态
	
	QString DataFileName;	//临时变量用于记录文件名
	int	   DataFileSize;	//同上，用于记录文件大小
	bool	   lastIsRootDir = true;		//记录是否根目录
	
	QRect m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
	bool m_bPressed;//鼠标按下标志（不分左右键）
	QPoint m_ptPress;//鼠标按下的初始位置
private slots:
	void addItem(int number , QString name, QString pro, QString size);
	void removeItem(int number,QString name);
	
	void listFileAndDir(QString path);	//遍历指定目录下的文件
	
	void uploadFile();				//上传，下载，删除
	void downloadFile();
	void deleteFile();
	
	void LocalDoubleClick();		//双击本地方文件夹
	void TargetDoubleClick();	//双目标文件夹
	
	void leave();		//socket断开后的处理
	
	void exitWindows();		//退出按钮
	void mixWindows();		//最小化按钮
	
	void tablewidegtSet(QTableWidget *tableWidget);	//处理两个tablewidger的界面
	void buttonSet(QPushButton* button);			//处理按钮的界面
	void setAreaMovable(const QRect rt);		//  自绘 窗口栏
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *event)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
		QWidget::paintEvent(event);
	    QPainter painter(this);
	    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
	    painter.setBrush(QBrush(Qt::white));
	    painter.setPen(Qt::transparent);
	    QRect rect = this->rect();
	    rect.setWidth(rect.width() - 1);
	    rect.setHeight(rect.height() - 1);
	    painter.drawRoundedRect(rect, 20, 20);
//	    QMainWindow::paintEvent(event);
	    QWidget::paintEvent(event);
	}
	
signals:
	void sendGetListFile(QString path);
	void dissConnectOpenFile(File *);		//此信号告诉MainWIndow已经绑定socket了，不需要再绑定了
};

#endif // FILE_H
