#ifndef THREAD_FFMPEG_LALIU_H
#define THREAD_FFMPEG_LALIU_H

#define WIDTH 1920
#define HEIGHT 1080
#define TIME_M 0

#include <QObject>
#include <QThread>
#include <QTime>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QElapsedTimer>

//声明引用C的头文件
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class thread_ffmpeg_laliu :public QObject
{
    Q_OBJECT
public:

    explicit thread_ffmpeg_laliu(QObject *parent=nullptr);//构造函数

    void Thread_Fun(void);

signals:
    void VideoDataOutput(QImage); //输出信号
};


#endif // THREAD_FFMPEG_LALIU_H
