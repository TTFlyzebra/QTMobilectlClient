#include "FlyPlayer.h"
#include <QDebug>
#include <QMutex>

extern "C" {
#include "libavutil/imgutils.h"
}

FlyPlayer::FlyPlayer(char* video_url)
    :mVideo_url(video_url)
    , is_stop(false)
    , out_sampleRateInHz(48000)
    , out_channelConfig(AV_CH_LAYOUT_STEREO)
    , out_audioFormat(AV_SAMPLE_FMT_S16)
{
}

FlyPlayer::~FlyPlayer()
{
}

int FlyPlayer::interrupt_cb(void* ctx)
{
    FlyPlayer* p = (FlyPlayer*)ctx;
    if (p->is_stop) {
        qDebug()<<"FlyFFmpeg interrupt_cb, will exit! ";
        return 1;
    }
    return 0;
}

void FlyPlayer::close()
{
    is_stop = true;
}

void FlyPlayer::run()
{
    int videoStream = -1;
    int audioStream = -1;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    //pFormatCtx->interrupt_callback.callback = interrupt_cb;
    //pFormatCtx->interrupt_callback.opaque = pFormatCtx;

    AVDictionary* avdic = NULL;
    av_dict_set(&avdic, "rtsp_transport", "tcp", 0);
    //av_dict_set(&avdic, "probesize", "2048", 0);
    //av_dict_set(&avdic, "max_analyze_duration", "1000000", 0);
    qDebug("open file %s", mVideo_url);
    int ret = avformat_open_input(&pFormatCtx, mVideo_url, nullptr, &avdic);
    //av_dict_free(&avdic);
    if (ret != 0) {
        qDebug("Couldn't open file %s: (ret:%d)", mVideo_url, ret);
        return ;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        qDebug("Could't find stream infomation.");
        return;
    }

    int totalSec = static_cast<int>(pFormatCtx->duration / AV_TIME_BASE);
    qDebug("play time [%d][%dmin%dsec].", pFormatCtx->duration, totalSec / 60, totalSec % 60);

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            qDebug("codec_id=%d, extradata_size=%d", pFormatCtx->streams[i]->codecpar->codec_id, pFormatCtx->streams[i]->codecpar->extradata_size);            
            int32_t v_format = pFormatCtx->streams[i]->codecpar->codec_id;
            int32_t v_width = pFormatCtx->streams[i]->codecpar->width;
            int32_t v_height = pFormatCtx->streams[i]->codecpar->height;
            qDebug("v_format=%d, v_width=%d, v_height=%d", v_format, v_width, v_height);

            AVCodecParameters* pCodecPar_video = pFormatCtx->streams[i]->codecpar;
            AVCodec* pCodec_video = avcodec_find_decoder(pCodecPar_video->codec_id);

            if (pCodec_video != nullptr) {
                pCodecCtx_video = avcodec_alloc_context3(pCodec_video);
                ret = avcodec_parameters_to_context(pCodecCtx_video, pCodecPar_video);
                if (ret >= 0) {
                    if (avcodec_open2(pCodecCtx_video, pCodec_video, nullptr) >= 0) {  
                        v_frame = av_frame_alloc();
                        uint8_t*  buffer = (uint8_t*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx_video->width, pCodecCtx_video->height, 1));
                        av_image_fill_arrays(
                            v_frame->data,                  // dst data[]
                            v_frame->linesize,              // dst linesize[]
                            buffer,                         // src buffer
                            AV_PIX_FMT_YUV420P,             // pixel format
                            pCodecCtx_video->width,         // width
                            pCodecCtx_video->height,        // height
                            1                               // align
                        );
                        sws_ctx = sws_getContext(
                            pCodecCtx_video->width,         // src width
                            pCodecCtx_video->height,        // src height
                            pCodecCtx_video->pix_fmt,       // src format
                            pCodecCtx_video->width,         // dst width
                            pCodecCtx_video->height,        // dst height
                            AV_PIX_FMT_YUV420P,             // dst format
                            SWS_BICUBIC,                    // flags
                            NULL,                           // src filter
                            NULL,                           // dst filter
                            NULL                            // param
                        );                  
                        videoStream = i;
                        break;
                    }
                    else {
                        qDebug("Could not open video decodec.");
                    }
                }
                else {
                    qDebug("avcodec_parameters_to_context() failed %d", ret);
                }
            }
            else {
                qDebug(" not found video decodec.");
            }
        }
    }

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVCodecParameters* pCodecPar_audio = pFormatCtx->streams[i]->codecpar;
            AVCodec* pCodec_audio = avcodec_find_decoder(pCodecPar_audio->codec_id);
            if (pCodec_audio != nullptr) {
                pCodecCtx_audio = avcodec_alloc_context3(pCodec_audio);
                ret = avcodec_parameters_to_context(pCodecCtx_audio, pCodecPar_audio);
                if (ret >= 0) {
                    if (avcodec_open2(pCodecCtx_audio, pCodec_audio, nullptr) >= 0) {
                        qDebug("find index = %d, sampleRate= %d, channel=%d, format=%d", i, pCodecCtx_audio->sample_rate, pCodecCtx_audio->channel_layout, pCodecCtx_audio->sample_fmt);
                        int in_ch_layout = pCodecCtx_audio->channel_layout;
                        if (pCodecCtx_audio->sample_fmt == 1) {
                            if (pCodecCtx_audio->channels > 1) {
                                in_ch_layout = AV_CH_LAYOUT_STEREO;
                            }
                            else {
                                in_ch_layout = AV_CH_LAYOUT_MONO;
                            }
                        }
                        swr_ctx = swr_alloc();
                        swr_alloc_set_opts(
                            swr_ctx,
                            out_channelConfig,
                            (AVSampleFormat)out_audioFormat,
                            out_sampleRateInHz,
                            in_ch_layout,
                            pCodecCtx_audio->sample_fmt,
                            pCodecCtx_audio->sample_rate,
                            0,
                            nullptr);
                        swr_init(swr_ctx);
                        audio_buf = (uint8_t*)av_malloc(out_sampleRateInHz * 16);
                        audioStream = i;
                        break;
                    }
                    else {
                        avcodec_close(pCodecCtx_audio);
                        qDebug("init audio codec failed 3!");
                    }
                }
                else {
                    qDebug("init audio codec failed 2!");
                }
            }
            else {
                qDebug("init audio codec failed 1!");
            }
        }
    }

    if (videoStream == -1 && audioStream == -1) {
        qDebug("not find vedio stream or audio stream.");
        return;
    }

    frame = av_frame_alloc();
    packet = (AVPacket*)av_malloc(sizeof(AVPacket)); 
    while (!is_stop && av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoStream) {
            //Èí½âÊÓÆµ
            ret = avcodec_send_packet(pCodecCtx_video, packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx_video, frame);
                if (ret >= 0) {
                    sws_scale(
                        sws_ctx,                                    // sws context
                        (const uint8_t* const*)frame->data,         // src slice
                        frame->linesize,                            // src stride
                        0,                                          // src slice y
                        pCodecCtx_video->height,                    // src slice height
                        v_frame->data,                              // dst planes
                        v_frame->linesize                           // dst strides
                    );
                    v_frame->width = frame->width;
                    v_frame->height = frame->height;
                    auto* video_buf = (uchar*)malloc((v_frame->width * v_frame->height * 3 / 2) * sizeof(uchar));
                    int start = 0;
                    memcpy(video_buf, v_frame->data[0], v_frame->width * v_frame->height);
                    start = start + v_frame->width * v_frame->height;
                    memcpy(video_buf + start, v_frame->data[1], v_frame->width * v_frame->height / 4);
                    start = start + v_frame->width * v_frame->height / 4;
                    memcpy(video_buf + start, v_frame->data[2], v_frame->width * v_frame->height / 4);
                    qDebug("Video-yuv data [%d]", v_frame->width* v_frame->height * 3 / 2);
                    free(video_buf);                    
                }
            }
        }
        else if (packet->stream_index == audioStream) {
            ret = avcodec_send_packet(pCodecCtx_audio, packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx_audio, frame);
                if (ret >= 0) {
                    int64_t delay = swr_get_delay(swr_ctx, frame->sample_rate);
                    int64_t out_count = av_rescale_rnd(
                        frame->nb_samples + delay,
                        out_sampleRateInHz,
                        frame->sample_rate,
                        AV_ROUND_UP);
                    int retLen = swr_convert(
                        swr_ctx,
                        &audio_buf,
                        out_count,
                        (const uint8_t**)frame->data,
                        frame->nb_samples);
                    if (retLen > 0) {
                        //callBack->javaOnAudioDecode(audio_buf, retLen * 4);
                        qDebug("Audio-pcm data [%d]", retLen*4);
                    }
                    else {
                        qDebug("frame->linesize[0]=%d, frame->nb_samples=%d,retLen=%d, delay=%lld,out_count=%lld", frame->linesize[0], frame->nb_samples, retLen, delay, out_count);
                    }

                }
            }
        }
        av_packet_unref(packet);
    }
    if (swr_ctx) {
        qDebug("swr_free swr_cxt.");
        swr_free(&swr_ctx);
    }
    if (sws_ctx) {
        qDebug("swr_free swr_cxt.");
        sws_freeContext(sws_ctx);
    }
    if (audio_buf) {
        qDebug("av_free audio_buf.");
        av_free(audio_buf);
    }
    if (packet) {
        qDebug("av_free packet.");
        av_free(packet);
    }
    if (frame) {
        qDebug("av_frame_free frame.");
        av_frame_free(&frame);
    }
    if (v_frame) {
        qDebug("av_frame_free v_frame.");
        av_frame_free(&v_frame);
    }
    if (pCodecCtx_video) {
        qDebug("avcodec_close pCodecCtx_video.");
        avcodec_close(pCodecCtx_video);
    }
    if (pCodecCtx_audio) {
        qDebug("avcodec_close pCodecCtx_audio.");
        avcodec_close(pCodecCtx_audio);
    }
    if (pFormatCtx) {
        qDebug("avformat_close_input pFormatCtx.");
        avformat_close_input(&pFormatCtx);
    }
    return;
}