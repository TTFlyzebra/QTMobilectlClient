#include "OpenGLWidget.h"
#define GET_GLSTR(x) #x

const char* vsrc = GET_GLSTR(
    attribute vec4 vertexIn; 
    attribute vec2 textureIn;
    varying vec2 textureOut; 
    void main(void)          
    {                        
        gl_Position = vertexIn; 
        textureOut = textureIn;
    }
);
const char* fsrc = GET_GLSTR(
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

OpenGLWidget::OpenGLWidget(QWidget* parent)
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

    fmt.setSampleRate(44100);  //设定播放采样频率为44100Hz的音频文件
    fmt.setSampleSize(16);     //设定播放采样格式（采样位数）为16位(bit)的音频文件。QAudioFormat支持的有8/16bit，即将声音振幅化为256/64k个等级
    fmt.setChannelCount(2);    //设定播放声道数目为2通道（立体声）的音频文件。mono(平声道)的声道数目是1，stero(立体声)的声道数目是2
    fmt.setCodec("audio/pcm"); //播放PCM数据（裸流）得设置编码器为"audio/pcm"。"audio/pcm"在所有的平台都支持，也就相当于音频格式的WAV,以线性方式无压缩的记录捕捉到的数据。如想使用其他编码格式 ，可以通过QAudioDeviceInfo::supportedCodecs()来获取当前平台支持的编码格式
    fmt.setByteOrder(QAudioFormat::LittleEndian); //设定字节序，以小端模式播放音频文件
    fmt.setSampleType(QAudioFormat::UnSignedInt); //设定采样类型。根据采样位数来设定。采样位数为8或16位则设置为QAudioFormat::UnSignedInt
    out = new QAudioOutput(fmt);    //创建QAudioOutput对象并初始化
    io = out->start(); //调用start函数后，返回QIODevice对象的地址
}

OpenGLWidget::~OpenGLWidget()
{
    qDebug() << __func__;
    if(yuvPtr) free(yuvPtr);
    if (out) delete out;
}

void OpenGLWidget::initializeGL()
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


void OpenGLWidget::resizeGL(int width, int height)
{
    qDebug() <<__func__<< "width=" << width << ",height=" << height;
}

void OpenGLWidget::paintGL()
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

void OpenGLWidget::upYuvDate(uchar* data, int32_t size)
{
    memcpy(yuvPtr, data, size);
    update();
}

void OpenGLWidget::upPcmDate(uchar* data, int32_t size)
{
    io->write((const char*)data, size);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << event <<"----" << event->pos();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}

