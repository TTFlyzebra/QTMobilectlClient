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

    fmt.setSampleRate(44100);  //�趨���Ų���Ƶ��Ϊ44100Hz����Ƶ�ļ�
    fmt.setSampleSize(16);     //�趨���Ų�����ʽ������λ����Ϊ16λ(bit)����Ƶ�ļ���QAudioFormat֧�ֵ���8/16bit���������������Ϊ256/64k���ȼ�
    fmt.setChannelCount(2);    //�趨����������ĿΪ2ͨ����������������Ƶ�ļ���mono(ƽ����)��������Ŀ��1��stero(������)��������Ŀ��2
    fmt.setCodec("audio/pcm"); //����PCM���ݣ������������ñ�����Ϊ"audio/pcm"��"audio/pcm"�����е�ƽ̨��֧�֣�Ҳ���൱����Ƶ��ʽ��WAV,�����Է�ʽ��ѹ���ļ�¼��׽�������ݡ�����ʹ�����������ʽ ������ͨ��QAudioDeviceInfo::supportedCodecs()����ȡ��ǰƽ̨֧�ֵı����ʽ
    fmt.setByteOrder(QAudioFormat::LittleEndian); //�趨�ֽ�����С��ģʽ������Ƶ�ļ�
    fmt.setSampleType(QAudioFormat::UnSignedInt); //�趨�������͡����ݲ���λ�����趨������λ��Ϊ8��16λ������ΪQAudioFormat::UnSignedInt
    out = new QAudioOutput(fmt);    //����QAudioOutput���󲢳�ʼ��
    io = out->start(); //����start�����󣬷���QIODevice����ĵ�ַ
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


void OpenGLWidget::resizeGL(int width, int height)
{
    qDebug() <<__func__<< "width=" << width << ",height=" << height;
}

void OpenGLWidget::paintGL()
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

