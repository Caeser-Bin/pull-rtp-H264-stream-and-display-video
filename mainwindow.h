#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define TIME_M 40

#include <QMainWindow>

#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTime>

#include "thread_ffmpeg_laliu.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
//声明引用C的头文件
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QThread *Thread_Test;
    thread_ffmpeg_laliu *thread_class;

signals:
    void ToThread(); // 信号


public slots:
    void VideoDataDisplay(QImage img);//接收图片
private slots:
    void on_startBtn_clicked();
    void on_saveBtn_clicked();
};
#endif // MAINWINDOW_H
