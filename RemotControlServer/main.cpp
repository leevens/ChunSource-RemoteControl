#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <qDebug>
//#pragma execution_character_set("utf-8")  

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	
	/*
	QString path = "d:/code/test";
	QString str = path.section("/",-1);
	qDebug()<<"str:"<<str;
	*/
	return a.exec();
}
