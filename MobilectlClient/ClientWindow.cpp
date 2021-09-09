#include <QThread>
#include "ClientWindow.h"


ClientWindow::ClientWindow(Controller* controller, QWidget* parent)
	:mController(controller)
	, QOpenGLWidget(parent)
	, videoW(720)
	, videoH(1280)
{
	qDebug() << __func__;
	ui.setupUi(this);
	yuvPtr = (uchar*)malloc((720 * 1280 * 3 / 2) * sizeof(uchar));

	memset(yuvPtr, 0x1D, 720 * 1280);
	memset(yuvPtr + 720 * 1280, 0x9D, 720 * 1280 / 4);
	memset(yuvPtr + 720 * 1280 * 5 / 4, 0x68, 720 * 1280 / 4);

	fmt.setSampleRate(48000);
	fmt.setSampleSize(16);
	fmt.setChannelCount(2);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	out = new QAudioOutput(fmt);
	io = out->start();
}

ClientWindow::~ClientWindow()
{
	qDebug() << __func__;
	if (yuvPtr) free(yuvPtr);
	if (mGLShaderProgram) mGLShaderProgram->release();
	if (mGLTextureY) mGLTextureY->release();
	if (out) {
		out->stop();
		delete out;
	}
	io = nullptr;
}

void ClientWindow::initializeGL()
{
	qDebug() << __func__;
	initializeOpenGLFunctions();
	//glEnable(GL_DEPTH_TEST);    

	static const GLfloat vertices[]{
		//��������
		-1.0f,-1.0f,
		-1.0f,+1.0f,
		+1.0f,+1.0f,
		+1.0f,-1.0f,
		//��������
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};
	mGLBuffer.create();
	mGLBuffer.bind();
	mGLBuffer.allocate(vertices, sizeof(vertices));
	QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceCode(vsrc);
	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceCode(fsrc);
	mGLShaderProgram = new QOpenGLShaderProgram(this);
	mGLShaderProgram->addShader(vshader);
	mGLShaderProgram->addShader(fshader);
	mGLShaderProgram->bindAttributeLocation("vertexIn", 0);
	mGLShaderProgram->bindAttributeLocation("textureIn", 1);
	mGLShaderProgram->link();
	mGLShaderProgram->bind();
	mGLShaderProgram->enableAttributeArray(0);
	mGLShaderProgram->enableAttributeArray(1);
	mGLShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	mGLShaderProgram->setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));
	textureUniformY = mGLShaderProgram->uniformLocation("tex_y");
	textureUniformU = mGLShaderProgram->uniformLocation("tex_u");
	textureUniformV = mGLShaderProgram->uniformLocation("tex_v");
	mGLTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureY->create();
	mGLTextureU->create();
	mGLTextureV->create();
	idY = mGLTextureY->textureId();
	idU = mGLTextureU->textureId();
	idV = mGLTextureV->textureId();

	glClearColor(0.0f, 0.3f, 0.7f, 1.0f);
}


void ClientWindow::resizeGL(int width, int height)
{
	qDebug() << __func__ << "width=" << width << ",height=" << height;
	mWidth = width;
	mHeigh = height;
}

void ClientWindow::paintGL()
{
	glActiveTexture(GL_TEXTURE0);  //��������ԪGL_TEXTURE0,ϵͳ�����
	glBindTexture(GL_TEXTURE_2D, idY); //��y�����������id�����������Ԫ
	//ʹ���ڴ��е����ݴ���������y������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1); //��������ԪGL_TEXTURE1
	glBindTexture(GL_TEXTURE_2D, idU);
	//ʹ���ڴ��е����ݴ���������u������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2); //��������ԪGL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, idV);
	//ʹ���ڴ��е����ݴ���������v������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH * 5 / 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//ָ��y����Ҫʹ����ֵ
	glUniform1i(textureUniformY, 0);
	//ָ��u����Ҫʹ����ֵ
	glUniform1i(textureUniformU, 1);
	//ָ��v����Ҫʹ����ֵ
	glUniform1i(textureUniformV, 2);
	//ʹ�ö������鷽ʽ����ͼ��
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

void ClientWindow::closeEvent(QCloseEvent* event)
{
	//QThread::usleep(20000);
}

void ClientWindow::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << event << "----" << event->pos();
	if (event->button() == Qt::LeftButton){
		lastX = event->pos().x() * 1080 / mWidth;
		lastY = event->pos().y() * 1920 / mHeigh;
		leftDown[12] = lastX >> 8 & 0x000000FF;
		leftDown[13] = lastX & 0x000000FF;
		leftDown[16] = lastY >> 8 & 0x000000FF;
		leftDown[17] = lastY & 0x000000FF;
		mController->sendCommand(leftDown, sizeof(leftDown));
	}else if (event->button() == Qt::RightButton){
		mController->sendCommand(key_back, sizeof(key_back));
	}else if (event->button() == Qt::MidButton) {
		mController->sendCommand(key_home, sizeof(key_home));
	}
	
}

void ClientWindow::mouseMoveEvent(QMouseEvent* event)
{
	//qDebug() << event << "----" << event->pos();
	if (event->buttons() == Qt::LeftButton) {
		int32_t x = event->pos().x() * 1080 / mWidth;
		int32_t y = event->pos().y() * 1920 / mHeigh;
		leftMove[12] = x >> 8 & 0x000000FF;
		leftMove[13] = x & 0x000000FF;
		leftMove[16] = y >> 8 & 0x000000FF;
		leftMove[17] = y & 0x000000FF;
		mController->sendCommand(leftMove, sizeof(leftMove));
		if (x <= 1080 && y <= 1920) {
			lastX = x;
			lastY = y;
		}
	}
}

void ClientWindow::mouseReleaseEvent(QMouseEvent* event)
{
	//qDebug() << event << "----" << event->pos();
	if (event->button() == Qt::LeftButton) {
		int32_t x = event->pos().x() * 1080 / mWidth;
		int32_t y = event->pos().y() * 1920 / mHeigh;
		if (x > 1080 || y > 1920) {
			x = lastX;
			y = lastY;
		}
		leftUp[12] = x >> 8 & 0x000000FF;
		leftUp[13] = x & 0x000000FF;
		leftUp[16] = y >> 8 & 0x000000FF;
		leftUp[17] = y & 0x000000FF;
		mController->sendCommand(leftUp, sizeof(leftUp));
	}
}

//signal
int32_t ClientWindow::upYuvDate(uchar* data, int32_t width, int32_t height, int32_t size)
{
	//qDebug(__func__);
	memcpy(yuvPtr, data, size);
	update();
	//qDebug()<< __func__ <<" End";
	return 0;
}

//signal
int32_t ClientWindow::upPcmDate(uchar* data, int32_t size)
{
	//qDebug("Write pcmData Start");
	if (io) io->write((const char*)data, size);
	//qDebug("Write pcmData End");
	return 0;
}

