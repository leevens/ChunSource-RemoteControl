#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
	class Keyboard;
}

class Keyboard : public QWidget
{
	Q_OBJECT
	
public:
	explicit Keyboard(QWidget *parent = 0);
	~Keyboard();
	void setSocket(QTcpSocket* socket,int id = 0);
	
private:
	Ui::Keyboard *ui;
	QTcpSocket *socket;
	
private slots:
	void leave();
	void processReturn();
	
signals:
	void dissConnectOpenFile(Keyboard *);
};

#endif // KEYBOARD_H
