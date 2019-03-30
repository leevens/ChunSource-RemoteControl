#ifndef SHELL_H
#define SHELL_H

#include <QWidget>
#include <QTcpSocket>
namespace Ui {
	class Shell;
}

class Shell : public QWidget
{
	Q_OBJECT
	
public:
	explicit Shell(QWidget *parent = 0);
	~Shell();
	
private:
	Ui::Shell *ui;
	QTcpSocket *socket;
public slots:
	void setSocket(QTcpSocket* socket,int id = 0);
	void leave();
	void processReturn();
	void sendMessage();
signals:
	void dissConnectOpenFile(Shell *);
};

#endif // SHELL_H
