#include <QThread>
#include "ClientWindow.h"


ClientWindow::ClientWindow(QWidget* parent)
    : QOpenGLWidget(parent)
    ,videoW(720)
    ,videoH(1280)
{
    qDebug() << __func__;
    ui.setupUi(this);
    yuvPtr = (uchar*)malloc((720*1280*3/2) * sizeof(uchar));

    memset(yuvPtr, 0x1D, 720 * 1280);
    memset(yuvPtr+720*1280, 0x9D, 720 * 1280  / 4);
    memset(yuvPtr+720*1280*5/4, 0x68, 720 * 1280 / 4);

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
    if(yuvPtr) free(yuvPtr);
    if (out) delete out;
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
    qDebug() <<__func__<< "width=" << width << ",height=" << height;
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
    QObject::disconnect(0, 0, this, 0);
    QThread::usleep(20000);
}


void ClientWindow::upYuvDate(uchar* data, int32_t size)
{
    memcpy(yuvPtr, data, size);
    update();
}

void ClientWindow::upPcmDate(uchar* data, int32_t size)
{
    io->write((const char*)data, size);
}

void ClientWindow::mousePressEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}

void ClientWindow::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << event <<"----" << event->pos();
}

void ClientWindow::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}

