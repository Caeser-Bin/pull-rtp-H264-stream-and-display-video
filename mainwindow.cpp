#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ʵ���������̣߳���ʼ���źŲ�
    Thread_Test = new QThread;
    thread_class = new thread_ffmpeg_laliu();
    //��ǰ��-�����߳��źš���thread_ffmpeg_laliu-�߳����к���
    connect(this,&MainWindow::ToThread,thread_class,&thread_ffmpeg_laliu::Thread_Fun);
    //thread_class��-��������ͼ���źš�����ǰ��-����ͼ���ź�
    connect(thread_class,SIGNAL(VideoDataOutput(QImage )),this,SLOT(VideoDataDisplay(QImage )));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//��ʼ����
/**
 * @brief MainWindow::on_startBtn_clicked
 * ���������߳�
 */
void MainWindow::on_startBtn_clicked()
{
    //��ǰϵͳʱ�䣨���룩
    qDebug()<<"0-start "<<QDateTime::currentDateTime().toMSecsSinceEpoch();

    thread_class->moveToThread(Thread_Test);
    Thread_Test->start();
    emit ToThread();
}


/**
 * @brief delay
 * @param msec����Ƶ���ʱ��  ��λ������
 * ��Ƶ���ʱ��
 */
void delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//������Ƶ֡
QImage img_photo;
/**
 * @brief MainWindow::VideoDataDisplay
 * @param img���̴߳����Ľ����ͼ��
 * ���ܣ�ֻ����
 * ȫ�ֱ���QImage img_photo����ͼ�õ�
 */
void MainWindow::VideoDataDisplay(QImage img)
{
    img_photo = img;
    QPixmap my_pixmap;
    my_pixmap.convertFromImage(img);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(my_pixmap);
    //delay(40);   // 2023/2/17/14:50ɾ��
}

/**
 * @brief MainWindow::on_saveBtn_clicked
 * ���ܣ���ǰͼ��浽����
 */
void MainWindow::on_saveBtn_clicked()
{
    //ϵͳ��ǰʱ�䣬���뼶
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd_hh_mm_ss");
    QString imgname = QString("./save/")+current_date+QString(".png");
    img_photo.save(imgname);

    qDebug()<<"click time"<<timestamp;
}
