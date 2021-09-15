#pragma once
#include "ui_ClientWindow.h"

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QAudioFormat>
#include <QAudioOutput>
#include "Controller.h"

class ClientWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ClientWindow(QWidget* parent = Q_NULLPTR);
	~ClientWindow();
	void setController(Controller* ctl);

public slots:
	int32_t upYuvDate(uchar* data, int32_t width, int32_t height, int32_t size);
public slots:
	int32_t upPcmDate(uchar* data, int32_t size);
private slots:
	void timerUpdate();

protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();
	virtual void closeEvent(QCloseEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);


private:
	Ui::ClientWindow ui;

	QOpenGLBuffer mGLBuffer;
	QOpenGLShaderProgram* mYuvShaderProgram;
	QOpenGLTexture* mTextures[3];

	//QOpenGLShaderProgram* mPngShaderProgram;	
	//QOpenGLTexture* png_texture;

	uchar* yuvPtr = nullptr;
	volatile int32_t v_width;
	volatile int32_t v_height;

	QAudioFormat fmt;
	QAudioOutput* out;
	QIODevice* io;

	Controller* mController;
	volatile int32_t mWidth;
	volatile int32_t mHeigh;

	QMutex mLock;

	uchar leftDown[28] = {
		0x02,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0x00,0x00,0x02,0x24,0x00,0x00,
		0x03,0xca,0x04,0x38,0x07,0x80,0x00,0x12,
		0x00,0x00,0x00,0x01 };

	uchar leftMove[28] = {
		0x02,0x02,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0x00,0x00,0x02,0x24,0x00,0x00,
		0x03,0xca,0x04,0x38,0x07,0x80,0x00,0x12,
		0x00,0x00,0x00,0x01 };

	uchar leftUp[28] = {
		0x02,0x01,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0x00,0x00,0x02,0x24,0x00,0x00,
		0x03,0xca,0x04,0x38,0x07,0x80,0x00,0x00,
		0x00,0x00,0x00,0x01 };

	uchar key_back[1] = { 0x04 };

	uchar key_home[14] = {
		0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00 };

	int32_t lastX, lastY;

	//QTimer* timer;
};

