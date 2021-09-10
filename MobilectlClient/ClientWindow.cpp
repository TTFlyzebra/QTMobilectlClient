#include <QThread>
#include "ClientWindow.h"


#define GET_GLSTR(x) #x

static const char* vsrc = GET_GLSTR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);
static const char* fsrc = GET_GLSTR(
	varying vec2 textureOut;
	uniform sampler2D tex_y;
	uniform sampler2D tex_u;
	uniform sampler2D tex_v;
	void main(void)
	{
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(tex_y, textureOut).r;
		yuv.y = texture2D(tex_u, textureOut).r - 0.5;
		yuv.z = texture2D(tex_v, textureOut).r - 0.5;
		rgb = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0) * yuv;
		gl_FragColor = vec4(rgb, 1);
	}
);


ClientWindow::ClientWindow(QWidget* parent)
	: QOpenGLWidget(parent)
	, v_width(720)
	, v_height(1280)
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
	qDebug("ClientWindow %s", __func__);
	if (yuvPtr) free(yuvPtr);
	if (mGLShaderProgram) mGLShaderProgram->release();
	if (out) {
		out->stop();
		delete out;
	}
	io = nullptr;
}

void ClientWindow::setController(Controller* controller)
{
	mController = controller;
}

void ClientWindow::initializeGL()
{
	qDebug("ClientWindow %s", __func__);
	initializeOpenGLFunctions();
	//glEnable(GL_DEPTH_TEST);  

	//QMatrix4x4 mat;
	//mat.setToIdentity();
	//QVector3D eye(10, 10, 10);
	//QVector3D cen(0, 0, 0);
	//QVector3D up(0, 1, 0);
	//mat.lookAt(eye, cen, up);

	static const GLfloat vertices[]{
		//顶点坐标
		-1.0f, -1.0f,
		-1.0f, +1.0f,
		+1.0f, +1.0f,
		+1.0f, -1.0f,
		//纹理坐标
		+0.0f, +1.0f,
		+0.0f, +0.0f,
		+1.0f, +0.0f,
		+1.0f, +1.0f,
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
	mUfY = mGLShaderProgram->uniformLocation("tex_y");
	mUfU = mGLShaderProgram->uniformLocation("tex_u");
	mUfV = mGLShaderProgram->uniformLocation("tex_v");

	mGLTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	mGLTextureY->create();
	mGLTextureU->create();
	mGLTextureV->create();
	mIdY = mGLTextureY->textureId();
	mIdU = mGLTextureU->textureId();
	mIdV = mGLTextureV->textureId();
	mGLTextureY->release();
	mGLTextureU->release();
	mGLTextureV->release();

	mGLBuffer.release();
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
	glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的
	glBindTexture(GL_TEXTURE_2D, mIdY); //绑定y分量纹理对象id到激活的纹理单元
	//使用内存中的数据创建真正的y分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width, v_height, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUniform1i(mUfY, 0);//指定y纹理要使用新值

	glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
	glBindTexture(GL_TEXTURE_2D, mIdU);
	//使用内存中的数据创建真正的u分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width >> 1, v_height >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + v_width * v_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUniform1i(mUfU, 1);//指定u纹理要使用新值

	glActiveTexture(GL_TEXTURE2); //激活纹理单元GL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, mIdV);
	//使用内存中的数据创建真正的v分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width >> 1, v_height >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + v_width * v_height * 5 / 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUniform1i(mUfV, 2);//指定v纹理要使用新值
	 
	//使用顶点数组方式绘制图形
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void ClientWindow::closeEvent(QCloseEvent* event)
{
	//QThread::usleep(20000);
}

void ClientWindow::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << event << "----" << event->pos();
	if (event->button() == Qt::LeftButton) {
		lastX = event->pos().x() * 1080 / mWidth;
		lastY = event->pos().y() * 1920 / mHeigh;
		leftDown[12] = lastX >> 8 & 0x000000FF;
		leftDown[13] = lastX & 0x000000FF;
		leftDown[16] = lastY >> 8 & 0x000000FF;
		leftDown[17] = lastY & 0x000000FF;
		mController->sendCommand(leftDown, sizeof(leftDown));
	}
	else if (event->button() == Qt::RightButton) {
		mController->sendCommand(key_back, sizeof(key_back));
	}
	else if (event->button() == Qt::MidButton) {
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
		if (x >= 0 && x <= 1080 && y <= 1920 && y >= 0) {
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
		if (x < 0 || x > 1080 || y < 0 || y > 1920) {
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

