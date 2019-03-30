#include "file.h"
#include "ui_file.h"
#include <qDebug>
//#include <windows.h>

File::File(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::File)
{
	qDebug()<<"FILE::FILE()";
	ui->setupUi(this);
	setWindowFlags( Qt::FramelessWindowHint );		//无边框
	setAttribute(Qt::WA_TranslucentBackground);
	tablewidegtSet(ui->TableLocal);
	tablewidegtSet(ui->TableTarget);
	buttonSet(ui->ButtonDelete);
	buttonSet(ui->ButtonDownLoad);
	buttonSet(ui->ButtonUpLoad);
	
	ui->LabelLocalPath->clear();
	ui->LabelTargetPath->clear();
	ui->progressBar->hide();
	ui->TableLocal->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->TableLocal->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->TableLocal->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->TableLocal->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->TableLocal->installEventFilter(this);
	
	ui->TableTarget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->TableTarget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->TableTarget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->TableTarget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->TableTarget->installEventFilter(this);
	this->setWindowTitle(QString::fromLocal8Bit("FileControl"));
	
	connect(ui->TableLocal,&QTableWidget::cellDoubleClicked,this,&File::LocalDoubleClick);
	connect(ui->TableTarget,&QTableWidget::cellDoubleClicked,this,&File::TargetDoubleClick);
	connect(ui->ButtonUpLoad,&QPushButton::clicked,this,&File::uploadFile);
	connect(ui->ButtonDelete,&QPushButton::clicked,this,&File::deleteFile);
	connect(ui->ButtonDownLoad,&QPushButton::clicked,this,&File::downloadFile);
	foreach (QFileInfo my_info, QDir::drives())
		{
			addItem(2,my_info.absoluteFilePath(),"dir","0");
		}
		m_areaMovable.setRect(0,0,991,41);
		m_bPressed = false;
}

File::~File()
{
	qDebug()<<"FILE 析构函数";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	delete ui;
}


void File::processReturn()
{
	
	if(this->CurrentStatus == true)
		{
			QByteArray buf = this->socket->readAll();
			int showSize = buf.size()/this->DataFileSize*100;
			ui->progressBar->setValue(showSize);
			qDebug()<<"write file:"<<buf;
			writeFile(this->DataFileName,buf);
			//	qDebug()<<"write file size:"<<buff.size();
			this->socket->waitForBytesWritten(1500);
			
			QFileInfo info(this->DataFileName);
			if(info.size() == this->DataFileSize)
				{
					this->CurrentStatus = false;
					qDebug()<<"退出读取文件模式";
					ui->progressBar->hide();
				}
			return;
		}
	
	char myCharbuff[4096];
	this->socket->read(myCharbuff,4096);
	
	wchar_t *t = new wchar_t[4096];
	t = Thread::char2wchar(myCharbuff);
	
	QString buf;
	buf = QString::fromWCharArray(t);
	qDebug()<<"unicode : "<<buf;
	
	buf = buf.section("&",0,0);
	qDebug()<<"READ FILE BUF:"<<buf;
	QStringList list = buf.split("#");
	if(list.size() == 0)
		{
			return;
		}
	QString head = list.at(0);
	
	if(head == "SEND_FILELIST_FRIST")
		{
			for(int i=1;i<list.size();i++)
				{
					QString buf = list.at(i);
					if(!buf.isEmpty())
						{
							addItem(1,list.at(i),"dir","0");
						}
				}
			return;
		}
	
	if(head == "SEND_FILELIST")	//接受对面的列出目录
		{
			ui->TableTarget->setRowCount(0);
			ui->TableTarget->clearContents();
			QStringList nameList,proList,sizeList,endList;
			QString buff = list.at(1);
			endList = buff.split(";");
			
			buff =  endList.at(0);
			nameList = buff.split("|");
			buff =  endList.at(1);
			proList = buff.split("|");
			buff =  endList.at(2);
			sizeList = buff.split("|");
			
			qDebug()<<"list name:"<<nameList;
			qDebug()<<"list proList:"<<proList;
			qDebug()<<"list sizeList:"<<sizeList;
			addItem(1,"../","dir",0);
			for(int i=0; i<nameList.size()-1;i++)
				{
					QString s = nameList.at(i);
					//	QString last = QString::from
					addItem(1,s,sizeList.at(i),proList.at(i));
				}
			return;
			
			
		}
	if(head == "SEND_FILE")
		{
			//	QString fileName = list.at(1);
			QString size = list.at(1);
			this->DataFileSize = size.toInt();
			this->CurrentStatus = true;	
			qDebug()<<"进入读取文件模式";
			ui->progressBar->show();
		}
}

void File::writeFile(QString path, QByteArray buf)
{
	QFile file(path);
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	file.write(buf);
	file.close();
}

void File::writeFile(QString path, char *buf)
{
	QFile file(path);
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	file.write(buf);
	file.close();
}

void File::setSocket(QTcpSocket *socket, int id)
{
	qDebug()<<"正在设置文件端的socket";
	this->socket = socket;
	connect(this->socket,&QTcpSocket::readyRead,this,&File::processReturn);
	connect(this->socket,&QTcpSocket::disconnected,this,&File::leave);
	emit dissConnectOpenFile(this);
}

void File::addItem(int number, QString name,QString pro,QString size)
{
	//	name = name.toLocal8Bit();
	if(number == 1)
		{
			
			int count = ui->TableTarget->rowCount();
			ui->TableTarget->setRowCount(count+1);
			QTableWidgetItem *itemName = new QTableWidgetItem(name);
			ui->TableTarget->setItem(count, 0 , itemName);
			QTableWidgetItem *itemPro = new QTableWidgetItem(pro);
			ui->TableTarget->setItem(count, 1 , itemPro);
			QTableWidgetItem *itemSize = new QTableWidgetItem(size);
			ui->TableTarget->setItem(count, 2 , itemSize);
		}
	if(number == 2)
		{
			
			int count = ui->TableLocal->rowCount();
			ui->TableLocal->setRowCount(count+1);
			QTableWidgetItem *itemName = new QTableWidgetItem(name);
			ui->TableLocal->setItem(count, 0 , itemName);
			QTableWidgetItem *itemPro = new QTableWidgetItem(pro);
			ui->TableLocal->setItem(count, 1 , itemPro);
			QTableWidgetItem *itemSize = new QTableWidgetItem(size);
			ui->TableLocal->setItem(count, 2 , itemSize);
		}
}

void File::removeItem(int number, QString name)
{
	if(number == 1)
		{
			
			int count = ui->TableTarget->rowCount();
			for (int i =0; i< count; ++i) {
					if (ui->TableTarget->item(i, 0)->text() == name)
						{
							ui->TableTarget->removeRow(i);
							break;
						}
				}
		}
	if(number == 2)
		{
			
			int count = ui->TableLocal->rowCount();
			for (int i =0; i< count; ++i) {
					if (ui->TableLocal->item(i, 0)->text() == name)
						{
							ui->TableLocal->removeRow(i);
							break;
						}
				}
		}
}

void File::listFileAndDir(QString path)
{
	QDir dir(path);
	
	foreach(QFileInfo mfi ,dir.entryInfoList())
		{
			if(mfi.isFile())
				{
					addItem(2,mfi.fileName(),"file",QString::number(mfi.size()));
				}else
				{
					if(mfi.fileName()=="." || mfi.fileName() == "..")
						continue;
					addItem(2,mfi.absoluteFilePath(),"dir","0");
				}
		}
}

void File::uploadFile()
{		
	
	/*
	*
	*		UPLOAD_FILE&FileName#FileSize
	*
	*
	*/
	
	QString fileName = ui->TableLocal->item(ui->TableLocal->currentRow(),0)->text();
	QFileInfo info(this->MyPath+fileName);
	int fileSize = info.size();
	QString buf="UPLOAD_FILE#";
	buf += this->TargetPath+fileName;
	buf +="#";
	buf += QString::number(fileSize);
	buf +="&";		//上传的文件的全部信息
	this->socket->write(buf.toLatin1());
	this->socket->waitForBytesWritten(500);
	
	QFile file(this->MyPath+fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray t = file.readAll();
	this->socket->write(t.data(),t.size());
	qDebug()<<"write t:"<<t;
	file.close();
	this->socket->waitForBytesWritten(1500);
}

void File::downloadFile()
{
	
	//	this->CurrentStatus = true;
	QString fileName = ui->TableTarget->item(ui->TableTarget->currentRow(),0)->text();
	this->DataFileName = this->MyPath + fileName;
	QString buf = "SEND_FILE";
	buf = buf +"#"+this->TargetPath+fileName+"&";
	this->socket->write(buf.toLatin1());
}

void File::deleteFile()
{
	QString fileName = ui->TableTarget->item(ui->TableTarget->currentRow(),0)->text();
	QString buf = "DELETE_FILE#";
	buf = buf +fileName+"&";
	this->socket->write(buf.toLatin1());
}

void File::LocalDoubleClick()
{
	QString fileName = ui->TableLocal->item(ui->TableLocal->currentRow(),0)->text();
	if(fileName == "../")
		{
			QString spl = this->MyPath.section("/",-1);		//返回上一目录，去除当前目录名
			this->MyPath = this->MyPath.remove(spl);
			
			if(this->MyPath.count("/") == 1 && spl.isEmpty())		//判断上级目录是否为根目录
				{
					ui->TableLocal->setRowCount(0);
					ui->TableLocal->clearContents();
					foreach (QFileInfo my_info, QDir::drives())	//如果是则遍历所有驱动器
						{
							addItem(2,my_info.absoluteFilePath(),"dir","0");
						}
					return;
				}
		}
	else
		{
			this->MyPath = fileName;				//不是则普通的返回上级目录,并重新设置目录变量
		}	
	
	
	
	QFileInfo info(this->MyPath);
	
	if(info.isDir())
		{
			ui->TableLocal->setRowCount(0);
			ui->TableLocal->clearContents();
			addItem(2,"../","dir",0);		//添加返回上级目录选项
			qDebug()<<"MyPath"<<this->MyPath;
			listFileAndDir(this->MyPath);
		}
	ui->LabelLocalPath->setText(this->MyPath);
}

void File::TargetDoubleClick()		//目标文件操作
{
	QString fileName = ui->TableTarget->item(ui->TableTarget->currentRow(),0)->text();
	QString filePro = ui->TableTarget->item(ui->TableTarget->currentRow(),1)->text();
	
	if(filePro != "dir")
		{
			QMessageBox::information(this,"Error","your choes no dir");
		}
	
	if(fileName == "../")		//判断是否为上级目录
		{
			QString spl = this->TargetPath.section("/",-1);		//取得上级目录的绝对路径
			this->TargetPath = this->TargetPath.remove(spl);
			
			if(this->TargetPath.count("/") == 1 && spl.isEmpty())		//如果上级目录为根目录，则获取所有驱动器
				{
					ui->TableLocal->setRowCount(0);
					ui->TableLocal->clearContents();
					this->socket->write("SEND_FILELIST_FRIST");
					ui->LabelTargetPath->setText(this->TargetPath);
					return;
				}
		}
	else		//如果不是选择上级目录则获取子目录
		{
			this->TargetPath = fileName;		
			QString buf = "SEND_FILELIST";
			buf = buf+ "#" + fileName + "&";
			this->socket->write(buf.toLatin1());
			ui->LabelTargetPath->setText(this->TargetPath);
			return;
		}	
	
	if(ui->TableTarget->item(ui->TableTarget->currentRow(),1)->text() == "dir")		//获取子目录
		{
			this->TargetPath = this->TargetPath+fileName +"/";
			QString buf = "SEND_FILELIST";
			buf = buf+ "#" + fileName + "&";
			this->socket->write(buf.toLatin1());
			qDebug()<<"write buf :"<<buf.toLatin1();
		}
	ui->LabelTargetPath->setText(this->TargetPath);
}

void File::leave()			//断开连接后的处理函数
{
	qDebug()<<"Leave()";
	this->socket->disconnectFromHost();
	this->socket->deleteLater();
	this->close();
}

void File::exitWindows()
{
	this->leave();
}

void File::mixWindows()
{
	this->minimumSize();
}

void File::tablewidegtSet(QTableWidget *tableWidget)
{
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget->installEventFilter(this);
	
	tableWidget->horizontalHeader()->setStretchLastSection(true);//设置充满表宽度
	tableWidget->verticalHeader()->setDefaultSectionSize(40);//设置行距
	tableWidget->setFrameShape(QFrame::NoFrame);//设置无边框
	tableWidget->setShowGrid(false); //设置不显示格子线
	tableWidget->verticalHeader()->setVisible(false);//设置垂直头不可见
	tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//可多选（Ctrl、Shift、 Ctrl+A都可以）
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选择行为时每次选择一行
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不可编辑
	tableWidget->horizontalHeader()->resizeSection(0,20);//设置表头第一列的宽度为150
	tableWidget->horizontalHeader()->resizeSection(2,50);//设置表头第一列的宽度为150
	tableWidget->horizontalHeader()->setFixedHeight(30);//设置表头的高度	
}

void File::buttonSet(QPushButton *button)
{
	button->setStyleSheet(
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
}
void File::mousePressEvent(QMouseEvent *e)
{
	//鼠标左键
	if(e->button() == Qt::LeftButton)
		{
			m_ptPress = e->pos();
			m_bPressed = m_areaMovable.contains(m_ptPress);
		}
}

void File::mouseMoveEvent(QMouseEvent *e)
{
	if(m_bPressed)
		{
			move(pos() + e->pos() - m_ptPress);
		}
}

void File::mouseReleaseEvent(QMouseEvent *)
{
	m_bPressed = false;
}

//设置鼠标按下的区域
void File::setAreaMovable(const QRect rt)
{
	if(m_areaMovable != rt)
		{
			m_areaMovable = rt;
		}
}
