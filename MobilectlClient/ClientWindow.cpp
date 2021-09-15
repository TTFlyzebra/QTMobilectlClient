#include <QThread>
#include "ClientWindow.h"

#define GET_GLSTR(x) #x

static const char* yuv_vsrc = GET_GLSTR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);
static const char* yuv_fsrc = GET_GLSTR(
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
	, mWidth(450)
	, mHeigh(800)
{
	qDebug() << __func__;
	ui.setupUi(this);

	yuvPtr = (uchar*)malloc((v_width * v_height * 3 / 2) * sizeof(uchar));

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

	if (out) {
		out->stop();
		delete out;
	}

	delete mYuvShaderProgram;
	for (int i = 0; i < sizeof(mTextures) / sizeof(mTextures[0]); i++) {
		delete mTextures[i];
	}	
}

void ClientWindow::setController(Controller* controller)
{
	mController = controller;
}

void ClientWindow::initializeGL()
{
	qDebug("ClientWindow %s", __func__);
	initializeOpenGLFunctions();
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const GLfloat vertices[]{
		//顶点坐标3 纹理坐标2
		-1.0f, -1.0f, +0.1f, +0.0f, +1.0f,
		-1.0f, +1.0f, +0.1f, +0.0f, +0.0f,
		+1.0f, +1.0f, +0.1f, +1.0f, +0.0f,
		+1.0f, -1.0f, +0.1f, +1.0f, +1.0f,
	};

	mGLBuffer.create();
	mGLBuffer.bind();
	mGLBuffer.allocate(vertices, sizeof(vertices));
	QOpenGLShader* yuv_vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	yuv_vshader->compileSourceCode(yuv_vsrc);
	QOpenGLShader* yuv_fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	yuv_fshader->compileSourceCode(yuv_fsrc);
	mYuvShaderProgram = new QOpenGLShaderProgram(this);
	mYuvShaderProgram->addShader(yuv_vshader);
	mYuvShaderProgram->addShader(yuv_fshader);
	mYuvShaderProgram->bindAttributeLocation("vertexIn",0);
	mYuvShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(float));
	mYuvShaderProgram->enableAttributeArray(0);
	mYuvShaderProgram->bindAttributeLocation("textureIn",1);
	mYuvShaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
	mYuvShaderProgram->enableAttributeArray(1);
	mYuvShaderProgram->link();
	mYuvShaderProgram->bind();
	mGLBuffer.release();

	for (int i = 0; i < sizeof(mTextures) / sizeof(mTextures[0]); i++) {
		mTextures[i] = new QOpenGLTexture(QOpenGLTexture::Target2D);
		mTextures[i]->create();
		mTextures[i]->release();
		glBindTexture(GL_TEXTURE_2D, mTextures[i]->textureId());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
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
	mYuvDataLock.lock();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextures[0]->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width, v_height, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	glUniform1i(mYuvShaderProgram->uniformLocation("tex_y"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mTextures[1]->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width >> 1, v_height >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + v_width * v_height);
	glUniform1i(mYuvShaderProgram->uniformLocation("tex_u"), 1);
	glActiveTexture(GL_TEXTURE2); 
	glBindTexture(GL_TEXTURE_2D, mTextures[2]->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_width >> 1, v_height >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + v_width * v_height * 5 / 4);
	glUniform1i(mYuvShaderProgram->uniformLocation("tex_v"), 2);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	mYuvDataLock.unlock();
}

void ClientWindow::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << event << "----" << event->pos();
	if (event->button() == Qt::LeftButton) {
		int32_t x = event->pos().x() * 1080 / mWidth;
		int32_t y = event->pos().y() * 1920 / mHeigh;
		if (x > 0 && x < 1080 && y < 1920 && y > 0) {
			leftDown[12] = x >> 8 & 0x000000FF;
			leftDown[13] = x & 0x000000FF;
			leftDown[16] = y >> 8 & 0x000000FF;
			leftDown[17] = y & 0x000000FF;
			mController->sendCommand(leftDown, sizeof(leftDown));
			lastX = x;
			lastY = y;
		}		
	}else if (event->button() == Qt::RightButton) {
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
		if (x > 0 && x < 1080 && y < 1920 && y > 0) {
			leftMove[12] = x >> 8 & 0x000000FF;
			leftMove[13] = x & 0x000000FF;
			leftMove[16] = y >> 8 & 0x000000FF;
			leftMove[17] = y & 0x000000FF;
			mController->sendCommand(leftMove, sizeof(leftMove));		
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
		if (x < 0 || x >= 1080 || y < 0 || y >= 1920) {
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

void ClientWindow::closeEvent(QCloseEvent* event)
{
	//QThread::usleep(20000);
}

//signal
int32_t ClientWindow::upYuvDate(uchar* data, int32_t width, int32_t height, int32_t size)
{
	mYuvDataLock.lock();
	if (v_width!=width || v_height != height || sizeof(yuvPtr)!=size) {
		v_width = width;
		v_height = height;
		free(yuvPtr);
		yuvPtr = (uchar*)malloc(size * sizeof(uchar));
	}
	//qDebug(__func__);
	if (yuvPtr) memcpy(yuvPtr, data, size);
	//qDebug()<< __func__ <<" End";
	mYuvDataLock.unlock();
	if (yuvPtr) update();
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

