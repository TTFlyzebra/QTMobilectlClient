#pragma once
#include <QThread>
#include <QtNetWork>
class Controller :public QThread
{
	Q_OBJECT

public:
	Controller();
	~Controller();
	void connect(char* ip_address);
	void sendCommand(uchar* command, int32_t size);
	void disconnect();

protected:
	virtual void run();


private slots:
	void recvData();

private:
	char mIpstr[256] = { 0 };
	int32_t mPort = 9008;	
	QTcpSocket* mTcpSocket;
	volatile bool is_stop;
	std::vector<uchar> sendBuf;
	QMutex mLock;
	QWaitCondition mTask;
};

