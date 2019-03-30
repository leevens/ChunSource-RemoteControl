#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "server.h"
#include "file.h"
#include "screen.h"
#include "keyboard.h"
namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	Ui::MainWindow *ui;
	Server *server;
	
	QRect m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
	bool m_bPressed;//鼠标按下标志（不分左右键）
	QPoint m_ptPress;//鼠标按下的初始位置
private slots:
	void addItem(int id , QString ip,QString name,QString systeminfo);	//添加登录信息
	void removeItem(int id);			//移除下线信息
	int CurrentClickID();			//获取点击的ID
	
	void startServer();		//开始 、 停止 服务
	void stopServer();
	
	void FileControl();		//文件控制
	void ScreenControl();	//屏幕监控
	void KeyBoard();		//键盘记录
	
	void dissConnect(File *f);	//断开信号连接
	void dissConnect(Screen *f);
	void dissConnect(Keyboard *f);
	
	void exit();				//关闭程序
	
	
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
	    QMainWindow::paintEvent(event);
	    QWidget::paintEvent(event);
	}
};

#endif // MAINWINDOW_H
