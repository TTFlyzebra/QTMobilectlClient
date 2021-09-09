#include "Controller.h"

Controller::Controller()
{
	qDebug(__func__);	
}

Controller::~Controller()
{
	qDebug(__func__);
}

void Controller::connect(char* ip_address)
{
	qDebug(__func__);
	sprintf(mIpstr, "%s", ip_address);
	is_stop = false;
	start();
}

void Controller::sendCommand(uchar* command, int32_t size)
{
	mLock.lock();
	if (sendBuf.size() > 1024) {
		sendBuf.clear();
	}
	sendBuf.insert(sendBuf.end(), command, command + size);
	mTask.wakeAll();
	mLock.unlock();
}

void Controller::run()
{
	mTcpSocket = new QTcpSocket();
	mTcpSocket->connectToHost(mIpstr,mPort);
	if (!mTcpSocket->waitForConnected(30000)){
		qDebug("Controller connectToHost failed");
		return;
	}
	QObject::connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(recvData()));
	while (!is_stop) {
		mLock.lock();
		if (sendBuf.empty()) {
			mTask.wait(&mLock);
		}
		if (!sendBuf.empty()) {
			const char* send = (const char*)&sendBuf[0];
			int32_t sendLen = mTcpSocket->write(send, sendBuf.size());
			mTcpSocket->flush();
			//qDebug("Controller send size[%d], errno[%d]", sendLen, errno);
			sendBuf.clear();
		}
		mLock.unlock();
	}
}
void Controller::recvData()
{
	char recvMsg[1024] = { 0 };
	int32_t recvLen = mTcpSocket->read(recvMsg, 1024);
	qDebug("Controller recv size[%d] errno[%d]", recvLen, errno);
	if (recvLen == -1){
		//TODO::
	}
}

void Controller::disconnect()
{
	is_stop = true;
	mLock.lock();
	mTask.wakeAll();
	mLock.unlock();
}
