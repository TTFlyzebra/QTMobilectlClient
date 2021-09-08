#pragma once
#include <QThread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class FlyPlayer:public QThread
{
    Q_OBJECT
public:
    FlyPlayer(char *video_url);
    ~FlyPlayer();
    void close();
    static int interrupt_cb(void* ctx);

protected:
    virtual void run();

signals:
    void yuv_signal(uchar* data, int32_t size);
    void pcm_signal(uchar* data, int32_t size);

private:
    volatile bool is_stop;
    char* mVideo_url;

    AVFormatContext* pFormatCtx = nullptr;
    AVCodecContext* pCodecCtx_video = nullptr;
    AVCodecContext* pCodecCtx_audio = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    struct SwrContext* swr_ctx = nullptr;
    AVFrame* v_frame = nullptr;
    struct SwsContext* sws_ctx;
    uint8_t* audio_buf = nullptr;
    int32_t out_sampleRateInHz;
    int32_t out_channelConfig;
    int32_t out_audioFormat;
};

