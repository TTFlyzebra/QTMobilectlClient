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
	sprintf(mIpstr, "%s", ip_address);
	is_stop = false;
	start();
}

void Controller::sendCommand(uchar* command, int32_t size)
{
	mYuvDataLock.lock();
	if (sendBuf.size() > 1024) {
		sendBuf.clear();
	}
	sendBuf.insert(sendBuf.end(), command, command + size);
	mTask.wakeAll();
	mYuvDataLock.unlock();
}

void Controller::run()
{
	mTcpSocket = new QTcpSocket();
	while (!is_stop)
	{	
		mTcpSocket->disconnect();
		mTcpSocket->abort();
		mTcpSocket->connectToHost(mIpstr, mPort);
		if (!mTcpSocket->waitForConnected(5000)){
			qDebug("Controller connectToHost failed");
			continue;
		}
		QObject::connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(recvData()));
		while (!is_stop) {
			mYuvDataLock.lock();
			if (sendBuf.empty()) {
				mTask.wait(&mYuvDataLock);
			}
			if (!sendBuf.empty()) {
				const char* send = (const char*)&sendBuf[0];
				int32_t sendLen = mTcpSocket->write(send, sendBuf.size());
				//qDebug("Controller send size[%d], errno[%d]", sendLen, errno);
				mTcpSocket->flush();				
				sendBuf.clear();
				if (sendLen < 0) break;
			}
			mYuvDataLock.unlock();
		}
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
	mYuvDataLock.lock();
	mTask.wakeAll();
	mYuvDataLock.unlock();
}
