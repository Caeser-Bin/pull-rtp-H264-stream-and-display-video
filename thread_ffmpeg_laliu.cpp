#include "thread_ffmpeg_laliu.h"
#include <QDebug>
#include <QTime>
#include <QElapsedTimer>

//声明引用C的头文件
extern "C"
{
#include <libavcodec/avcodec.h>     //编解码通用框架
#include <libavformat/avformat.h>   //处理音频、视频、字母封装&解封装通用框架
#include <libswscale/swscale.h>     //
#include <libavutil/imgutils.h>
}

//构造函数
thread_ffmpeg_laliu::thread_ffmpeg_laliu(QObject *parent) : QObject(parent)
{
    //qDebug()<<"Thread构造函数ID:"<<QThread::currentThreadId();
}

//运行ffmpeg拉流函数
void thread_ffmpeg_laliu::Thread_Fun()
{

    //注册FFMpeg的库
    av_register_all();
    //开始初始化
    qDebug()<<"1-thread start init"<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    AVFormatContext *pFormatCtx;                                // 存储音视频封装格式中包含的信息
    AVCodecContext *pCodecCtx;                                  // 视频流编码结构
    AVInputFormat *pInputFormat;                                // 输入文件的格式
    AVCodec *pCodec;                                            // 视频解码器
    AVFrame *pFrameYUV, *pFrameRGB;                                // 帧结构
    AVPacket *packet;                                           // 包结构
    struct SwsContext *img_convert_ctx;                         // 主要用于视频图像的转换
    unsigned char *out_buffer;                                  // 缓冲区
    int videoIndex = -1;                                        // 视频帧索引，初始化为-1
    int ret, got_picture;                                       // 状态变量


    /** *************毛龙彬****************************************/
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();                      // 初始化AVFormatContext 视频封装结构信息
    pFormatCtx ->flags |= AVFMT_NOFILE;
    //添加白名单，这里很重要，如果不申请内存，在avformat_close_input中会宕
    pFormatCtx ->protocol_whitelist =(char*) av_malloc(sizeof("file,udp,rtp"));
    memcpy(pFormatCtx ->protocol_whitelist,"file,udp,rtp",sizeof("file,udp,rtp"));

    char sdp_file_name[128] = "./gstreamer.sdp";
    //char sdp_file_name[128] = "./111.mp4";
    //输入的文件格式为sdp
    pInputFormat = av_find_input_format("sdp");

    /** ******************************************************** **/

    qDebug()<<"2-over start init || start registe ffmpeg"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    qDebug()<<"3-over registe ffmpeg || start init AVFormatContext"<<QDateTime::currentDateTime().toMSecsSinceEpoch();


    /** avformat_alloc_context()的注释
     * 分配 一个 AVFormatContext.
     * avformat_free_context() 可被用于 释放 context 以及 everything allocated by the framework within it.
     */
    //pFormatCtx = avformat_alloc_context();
    //qDebug()<<"thread time4"<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    //qDebug()<<"4-over init AVFormatContext || start set pFormatCtx"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    AVDictionary *pop = NULL;// 打开音视频流
    qDebug()<<"5-over set pFormatCtx || start avformat_open_input"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    av_dict_set(&pop,"rtsp_transport","tcp",0);
    if (avformat_open_input(&pFormatCtx, sdp_file_name,pInputFormat,&pop) != 0)//&pop -> nullptr
    {
        qDebug()<<"Couldn't open input stream.\n";
        exit(1);
    }
    qDebug()<<"6-over avformat_open_input || start avformat_find_stream_info"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

     //获取音视频流数据信息，打开此句设置nobuffer模式，图像质量不好但速度快
     //pFormatCtx->flags |= AVFMT_FLAG_NOBUFFER;

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)//生成视频流/获取视频流
    {
        qDebug()<<"Couldn't find stream information.\n";
        exit(1);
    }
    qDebug()<<"7-over avformat_find_stream_info || start find videoIndex"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 查找视频流的起始索引位置（nb_streams表示视音频流的个数）

    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        AVStream *avStream = pFormatCtx->streams[i];
        if(avStream->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            pCodecCtx = avcodec_alloc_context3(NULL);
            avcodec_parameters_to_context(pCodecCtx,avStream->codecpar);
            pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
            videoIndex = i;
            qDebug()<<"video index finded, the index is"<<i;
            break;//2023/2/17  17:09
        }
        qDebug()<<pFormatCtx->nb_streams;
    }

    if (videoIndex == -1)
    {
        qDebug()<<"Didn't find a video stream.\n";exit(1);
    }
//   2023/2/17 15:23删除

//    //2023/2/17 15:23新增
//    videoIndex = av_find_best_stream(pFormatCtx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
//    if(videoIndex<0)
//    {
//        qDebug()<<"没找到流";
//        exit(1);
//    }

    qDebug()<<"8-over find videoIndex || start find pCodecCtx"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 查找视频解码器// 获取视频流编码结构
    // 2023/2/17 15:57
    // pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
    // pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    qDebug()<<"9-over find pCodecCtx || start avcodec_open2"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
    {
        qDebug()<<"Could not open codec.\n";
        exit(1);
    }

    qDebug()<<"10-over avcodec_open2 || start set frame struct"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 打印视频信息
    av_dump_format(pFormatCtx, videoIndex, sdp_file_name, 0); // 此函数打印输入或输出的详细信息

    // 创建帧结构，此函数仅分配基本结构空间，图像数据空间需通过av_malloc分配
    pFrameYUV = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    qDebug()<<"11-over set frame struct || start set frame buffer"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 创建动态内存,创建存储图像数据的空间（av_image_get_buffer_size获取一帧图像需要的大小）
    out_buffer = (unsigned char *)av_malloc((size_t)av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1));
    qDebug()<<"12-over set frame buffer || start set frame array"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 存储一帧像素数据缓冲区
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer,AV_PIX_FMT_RGB32, WIDTH, HEIGHT, 1);
    qDebug()<<"13-over set frame array || start set frame packet"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 存放一帧数据的包
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    qDebug()<<"14-over set frame packet || start sws_getContext"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 初始化img_convert_ctx结构
    /** 对函数sws_getContext(...)的注释
      * @return 函数返回SwsContext结构体
      * @ SwsContext *sws_getContext(
      *                                 int srcW, //输入图像的宽
      *                                 int srcH, //输入图像的高
      *                                 enum AVPixelFormat srcFormat,//输入图像的颜色空间格式
      *                                 int dstW,//目标图像的宽
      *                                 int dstH,//目标图像的高
      *                                 enum AVPixelFormat dstFormat,//目标图像颜色空间格式
      *                                 int flags,//缩放算法
      *                                 SwsFilter *srcFilter,//输入图像滤波器信息
      *                                 SwsFilter *dstFilter,//输出图像滤波器信息
      *                                 const double *param//定义特定缩放算法需要的参数
      * );
      */
    img_convert_ctx = sws_getContext(WIDTH, HEIGHT, AV_PIX_FMT_YUV420P,WIDTH, HEIGHT, AV_PIX_FMT_RGB32, SWS_BICUBIC, nullptr, nullptr, nullptr);
    qDebug()<<"15-over sws_getContext || start av_read_frame"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    // av_read_frame读取一帧未解码的数据
    clock_t begin,end;
    clock_t time ;
    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        //qDebug()<<"16-over av_read_frame || start judge video"<<QDateTime::currentDateTime();
        begin = clock();
        // 如果是视频数据
        if (packet->stream_index == videoIndex)
        {
            //qDebug()<<"17-over judge video || start avcodec_decode_video2"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

            // 解码一帧视频数据，ret是函数的返回值，代表数据包大小
            ret = avcodec_decode_video2(pCodecCtx, pFrameYUV, &got_picture, packet);

            //运行完没有生成图片，25帧以后才有图片
            // qDebug()<<"ret"<<ret<<"pFrameYUV-pict_type"<<pFrameYUV->pict_type;//0：undefined 1:IDR帧 2:P帧
            // qDebug()<<"18-over avcodec_decode_video2 || start judge and got_picture"<<QDateTime::currentDateTime().toMSecsSinceEpoch()<<"ret"<<ret;

            if (ret < 0)
            {
                qDebug()<<"Decode Error.\n";
                return ;
            }
            if (got_picture)
            {
                //qDebug()<<"19-over judge and got_picture || start sws_scale"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

                //转码
                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrameYUV->data, pFrameYUV->linesize, 0, pCodecCtx->height,
                                    pFrameRGB->data, pFrameRGB->linesize);
                //qDebug()<<"20-over sws_scale || start turn to QImage"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

                QImage img((uchar*)pFrameRGB->data[0],pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                //qDebug()<<"21-over turn to QImage || start emit"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

                //帧序号
                // emit VideoPTSDataOutput(pFrameYUV->pts/99000);
                // qDebug() << QString("got one picture to emit use %1 ms").arg(strTime3);

                //发送图像
                emit VideoDataOutput(img);
                //qDebug()<<"22-over emit || start emit"<<QDateTime::currentDateTime().toMSecsSinceEpoch();

                //定时
                //QThread::msleep(TIME_M);
                //qDebug()<<"23-over msleep"<<QDateTime::currentDateTime().toMSecsSinceEpoch();
            }
        }
        end = clock();
        time = double(end-begin)/CLOCKS_PER_SEC*1000;
        if(time>=33)
            qDebug()<<"Time Used = "<<time<<"ms"<<endl;
        av_free_packet(packet);
    }
    qDebug()<<"24-over stamp"<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    /*** （四）最后要释放申请的内存空间 ***/
    sws_freeContext(img_convert_ctx);
    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrameYUV);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
