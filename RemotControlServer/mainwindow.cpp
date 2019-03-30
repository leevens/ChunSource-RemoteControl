#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->ButtonSystem->hide();
	setWindowFlags( Qt::FramelessWindowHint );		//无边框
	setAttribute(Qt::WA_TranslucentBackground);
	//	this->setStyleSheet("QWidget{border-top-left-radius:15px;border-top-right-radius:5px;border-top-left-radius:10px;border-bottom-left-radius: 10px;border-bottom-right-radius: 10px;}"); 
	
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->installEventFilter(this);
	
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);//设置充满表宽度
	ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);//设置行距
	ui->tableWidget->setFrameShape(QFrame::NoFrame);//设置无边框
	ui->tableWidget->setShowGrid(false); //设置不显示格子线
	ui->tableWidget->verticalHeader()->setVisible(false);//设置垂直头不可见
	ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//可多选（Ctrl、Shift、 Ctrl+A都可以）
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选择行为时每次选择一行
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不可编辑
	ui->tableWidget->horizontalHeader()->resizeSection(0,20);//设置表头第一列的宽度为150
	ui->tableWidget->horizontalHeader()->resizeSection(2,50);//设置表头第一列的宽度为150
	ui->tableWidget->horizontalHeader()->setFixedHeight(30);//设置表头的高度
	
	ui->ButtonStop->setStyleSheet(
				//正常状态样式
				"QPushButton{"
				"background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
				"border-style:outset;"                  //边框样式（inset/outset）
				"border-width:4px;"                     //边框宽度像素
				"border-radius:10px;"                   //边框圆角半径像素
				"border-color:rgba(255,255,255,30);"    //边框颜色
				"font:bold 13px;"                       //字体，字体大小
				"color:rgba(0,0,0,100);"                //字体颜色
				"padding:6px;"                          //填衬
				"}"
				//鼠标按下样式
				"QPushButton:pressed{"
				"background-color:rgba(100,255,100,200);"
				"border-color:rgba(255,255,255,30);"
				"border-style:inset;"
				"color:rgba(0,0,0,100);"
				"}"
				//鼠标悬停样式
				"QPushButton:hover{"
				"background-color:rgba(100,255,100,100);"
				"border-color:rgba(255,255,255,200);"
				"color:rgba(0,0,0,200);"
				"}");
	ui->ButtonStart->setStyleSheet(
				//正常状态样式
				"QPushButton{"
				"background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
				"border-style:outset;"                  //边框样式（inset/outset）
				"border-width:4px;"                     //边框宽度像素
				"border-radius:10px;"                   //边框圆角半径像素
				"border-color:rgba(255,255,255,30);"    //边框颜色
				"font:bold 13px;"                       //字体，字体大小
				"color:rgba(0,0,0,100);"                //字体颜色
				"padding:6px;"                          //填衬
				"}"
				//鼠标按下样式
				"QPushButton:pressed{"
				"background-color:rgba(100,255,100,200);"
				"border-color:rgba(255,255,255,30);"
				"border-style:inset;"
				"color:rgba(0,0,0,100);"
				"}"
				//鼠标悬停样式
				"QPushButton:hover{"
				"background-color:rgba(100,255,100,100);"
				"border-color:rgba(255,255,255,200);"
				"color:rgba(0,0,0,200);"
				"}");
	
	
	this->setWindowTitle(QString("2018-5-1 ChunSource远控").toUtf8());
	ui->ButtonStop->setDisabled(true);
	
	server = new Server();
	connect(ui->ButtonStart,&QPushButton::clicked,this,&MainWindow::startServer);
	connect(ui->ButtonStop,&QPushButton::clicked,this,&MainWindow::stopServer);
	connect(ui->EditPort,&QLineEdit::returnPressed,this,&MainWindow::startServer);
	connect(ui->ButtonFile,&QPushButton::clicked,this,&MainWindow::FileControl);
	connect(server,&Server::sendLoginInfo,this,&MainWindow::addItem);
	connect(server,&Server::sendLogoutInfo,this,&MainWindow::removeItem);
	connect(ui->ButtonExit,&QPushButton::clicked,this,&MainWindow::exit);
	connect(ui->ButtonScreen,&QPushButton::clicked,this,&MainWindow::ScreenControl);
	connect(ui->ButtonKeyboard,&QPushButton::clicked,this,&MainWindow::KeyBoard);
	m_areaMovable.setRect(0,0,1194,41);
	m_bPressed = false;
	
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::addItem(int id, QString ip, QString name, QString systeminfo)
{
	int count = ui->tableWidget->rowCount();
	ui->tableWidget->setRowCount(count+1);
	
	QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
	ui->tableWidget->setItem(count, 0 , itemId);
	
	QTableWidgetItem *itemIp = new QTableWidgetItem(ip);
	ui->tableWidget->setItem(count, 1 , itemIp );
	
	QTableWidgetItem *itemName = new QTableWidgetItem(name);
	ui->tableWidget->setItem(count, 2 , itemName);
	
	QTableWidgetItem *itemInfo = new QTableWidgetItem(systeminfo);
	ui->tableWidget->setItem(count, 3 , itemInfo);
}

void MainWindow::removeItem(int id)
{
	qDebug()<<"\nRemoveid:"<<id;
	int count = ui->tableWidget->rowCount();
	for (int i =0; i< count; ++i) {
			if (ui->tableWidget->item(i, 0)->text().toInt() == id)
				{
					ui->tableWidget->removeRow(i);
					break;
				}
		}
}

int MainWindow::CurrentClickID()
{
	if(ui->tableWidget->currentRow()==-1)
		{
			return -1;
		}
	else
		{
			int id = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().toInt();
			qDebug()<<"tables wieget  id:  "<<id;
			return id;
		}
}

void MainWindow::startServer()
{
	if(server->startServer(ui->EditPort->text().toInt()))
		{
			//QMessageBox::information(this,"Success","成功开启服务");
			qDebug()<<"Success 成功开启服务";
			ui->ButtonStart->setDisabled(true);
			ui->ButtonStop->setEnabled(true);
		}
	else
		{
			QMessageBox::information(this,"Error","服务开启失败");
		}
}

void MainWindow::stopServer()
{
	server->stopServer();
	ui->ButtonStart->setEnabled(true);
	ui->ButtonStop->setDisabled(true);
}

void MainWindow::FileControl()
{
	
	File *f = new File();
	f->show();
	
	int id = CurrentClickID();
	QTcpSocket *socket;
	for(int i =0;i<server->List.size();i++)
		{
			if(server->List.at(i).id == id)
				{
					//					socket->moveToThread(server->List.at(i).thread);
					socket = server->List.at(i).socket;
					socket->write("FILE&");
					break;
				}
		}
	
	connect(server,&Server::sendOpenFile,f,&File::setSocket);
	connect(f,SIGNAL(dissConnectOpenFile(File*)),this,SLOT(dissConnect(File*)));
	
}

void MainWindow::ScreenControl()
{
	Screen *s = new Screen();
	s->show();
	
	
	int id = CurrentClickID();
	QTcpSocket *socket;
	for(int i =0;i<server->List.size();i++)
		{
			if(server->List.at(i).id == id)
				{
					//					socket->moveToThread(server->List.at(i).thread);
					socket = server->List.at(i).socket;
					socket->write("SCREEN&");
					break;
				}
		}
	
	connect(server,&Server::sendOpenFile,s,&Screen::setSocket);
	//	connect(f,&File::dissConnectOpenFile,this,&MainWindow::dissConnect);
	connect(s,SIGNAL(dissConnectOpenFile(Screen*)),this,SLOT(dissConnect(Screen*)));
}

void MainWindow::KeyBoard()
{
	Keyboard *s = new Keyboard();
	s->show();
	
	
	int id = CurrentClickID();
	QTcpSocket *socket;
	for(int i =0;i<server->List.size();i++)
		{
			if(server->List.at(i).id == id)
				{
					//					socket->moveToThread(server->List.at(i).thread);
					socket = server->List.at(i).socket;
					socket->write("KEYBOARD&");
					break;
				}
		}
	
	connect(server,&Server::sendOpenFile,s,&Keyboard::setSocket);
	connect(s,SIGNAL(dissConnectOpenFile(Keyboard*)),this,SLOT(dissConnect(Keyboard*)));
}

void MainWindow::dissConnect(File *f)
{
	disconnect(server,&Server::sendOpenFile,f,&File::setSocket);
	//	disconnect(f,&File::dissConnectOpenFile,this,&MainWindow::dissConnect);
	disconnect(f,SIGNAL(dissConnectOpenFile(File*)),this,SLOT(dissConnect(File*)));
}

void MainWindow::dissConnect(Screen *s)
{
	disconnect(server,&Server::sendOpenFile,s,&Screen::setSocket);
	disconnect(s,SIGNAL(dissConnectOpenFile(Screen*)),this,SLOT(dissConnect(Screen*)));
}

void MainWindow::dissConnect(Keyboard *f)
{
	disconnect(server,&Server::sendOpenFile,f,&Keyboard::setSocket);
	disconnect(f,SIGNAL(dissConnectOpenFile(Keyboard*)),this,SLOT(dissConnect(Keyboard*)));
}

void MainWindow::exit()
{
	this->close();
}

//实现窗口移动
void MainWindow::mousePressEvent(QMouseEvent *e)
{
	//鼠标左键
	if(e->button() == Qt::LeftButton)
		{
			m_ptPress = e->pos();
			m_bPressed = m_areaMovable.contains(m_ptPress);
		}
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
	if(m_bPressed)
		{
			move(pos() + e->pos() - m_ptPress);
		}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
	m_bPressed = false;
}

//设置鼠标按下的区域
void MainWindow::setAreaMovable(const QRect rt)
{
	if(m_areaMovable != rt)
		{
			m_areaMovable = rt;
		}
}
