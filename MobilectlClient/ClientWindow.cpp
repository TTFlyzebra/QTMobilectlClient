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
        //顶点坐标
        -1.0f,-1.0f,
        -1.0f,+1.0f,
        +1.0f,+1.0f,
        +1.0f,-1.0f,
        //纹理坐标
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
    glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的
    glBindTexture(GL_TEXTURE_2D, idY); //绑定y分量纹理对象id到激活的纹理单元
    //使用内存中的数据创建真正的y分量纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D, idU);
    //使用内存中的数据创建真正的u分量纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2); //激活纹理单元GL_TEXTURE2
    glBindTexture(GL_TEXTURE_2D, idV);
    //使用内存中的数据创建真正的v分量纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr + videoW * videoH * 5 / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //指定y纹理要使用新值
    glUniform1i(textureUniformY, 0);
    //指定u纹理要使用新值
    glUniform1i(textureUniformU, 1);
    //指定v纹理要使用新值
    glUniform1i(textureUniformV, 2);
    //使用顶点数组方式绘制图形
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

