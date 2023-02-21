#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //实例化拉流线程，初始化信号槽
    Thread_Test = new QThread;
    thread_class = new thread_ffmpeg_laliu();
    //当前类-开启线程信号——thread_ffmpeg_laliu-线程运行函数
    connect(this,&MainWindow::ToThread,thread_class,&thread_ffmpeg_laliu::Thread_Fun);
    //thread_class类-输出解码后图像信号——当前类-接收图像信号
    connect(thread_class,SIGNAL(VideoDataOutput(QImage )),this,SLOT(VideoDataDisplay(QImage )));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//开始播放
/**
 * @brief MainWindow::on_startBtn_clicked
 * 开启拉流线程
 */
void MainWindow::on_startBtn_clicked()
{
    //当前系统时间（毫秒）
    qDebug()<<"0-start "<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    thread_class->moveToThread(Thread_Test);
    Thread_Test->start();
    emit ToThread();
}


/**
 * @brief delay
 * @param msec：视频间隔时间  单位：毫秒
 * 视频间隔时间
 */
void delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//接收视频帧
QImage img_photo;
/**
 * @brief MainWindow::VideoDataDisplay
 * @param img：线程传来的解码后图像
 * 功能：只播放
 * 全局变量QImage img_photo：存图用的
 */
void MainWindow::VideoDataDisplay(QImage img)
{
    img_photo = img;
    QPixmap my_pixmap;
    my_pixmap.convertFromImage(img);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(my_pixmap);
    //delay(40);   // 2023/2/17/14:50删除
}

/**
 * @brief MainWindow::on_saveBtn_clicked
 * 功能：当前图像存到本地
 */
void MainWindow::on_saveBtn_clicked()
{
    //系统当前时间，毫秒级
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd_hh_mm_ss");
    QString imgname = QString("./save/")+current_date+QString(".png");
    img_photo.save(imgname);

    qDebug()<<"click time"<<timestamp;
}
