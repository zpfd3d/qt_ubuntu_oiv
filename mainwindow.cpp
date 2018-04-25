#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QQueue>
#include <QDateTime>
#include <QKeyEvent>
#include <QProcess>
#include <QDir>
#include <QRegExp>
#include <unistd.h>
#include <time.h>
#include "system_info.h"
#include "ini_reader.h"
#include "aboutdialog.h"
#include "cameramanager.h"
#include "vislam_core.h"

QSerialPort serial;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("3VCam"));
    InitMotionTracker("dahua_960_fisheye.yaml", "ORBvoc.bin");//ALG
    setFixedSize(this->width(), this->height());
    Init_ToolBar();
    Init_StatusBar();
    Init_CameraParam();
    Ini_Reader::Ini_Read();
    Init_SerialPort();
    Init_IMUReader();//udp socket
    Camera_Connect();
    Init_Timer();
}

MainWindow::~MainWindow()
{
    delete ui;
}


extern float gPan,gTile,gX,gY,gZ;
extern bool gFlag_PoseDataReady;
void MainWindow::timerShowPoseData()
{
    if(gFlag_PoseDataReady){
        gFlag_PoseDataReady = false;

        ui->lineEdit_pose1->setText(QString("%1").arg(gPan));
        ui->lineEdit_pose2->setText(QString("%1").arg(gTile));
        ui->lineEdit_pose5->setText(QString("%1").arg(gX));
        ui->lineEdit_pose6->setText(QString("%1").arg(gY));
        ui->lineEdit_pose7->setText(QString("%1").arg(gZ));
    }
}

void MainWindow::Camera_Connect()
{
    QRect rc = ui->widgetCamera->geometry();
    manager = nullptr;
    manager = new CameraManager();
    manager->SetParent(ui->tabWidget->widget(0));
    int ret = manager->Start();
    if(3 == ret){//Two Tracking camera ok.
        lbl_HardWare_S1->setPixmap(QPixmap(":/res/Normal"));
        lbl_HardWare_S2->setPixmap(QPixmap(":/res/Normal"));
    }else if(1 == ret){//Only Tracking camera1 ok.
        lbl_HardWare_S1->setPixmap(QPixmap(":/res/Normal"));
    }else if(2 == ret){//Only Tracking camera2 ok.
        lbl_HardWare_S2->setPixmap(QPixmap(":/res/Normal"));
    }
    int x = rc.x()+rc.width()/2;
    int y = rc.y();
    ui->line_sep->setGeometry(x,y,1,rc.height());//seprator width:1

    manager->MoveWindow(rc);
    ui->tabWidget->removeTab(1);
}

void MainWindow::Init_Timer()
{
    //Timer for system info
    timerGetSystemInfo = new QTimer(this);
    connect(timerGetSystemInfo,SIGNAL(timeout()),this,SLOT(timerUpDateSysInfo()));
    timerGetSystemInfo->start(2000);//2s
    //Timer for Show Pose Data
    timerPoseData = new QTimer(this);
    connect(timerPoseData,SIGNAL(timeout()),this,SLOT(timerShowPoseData()));
    timerPoseData->start(15);//15ms
}

void MainWindow::timerUpDateSysInfo()
{
    int mem_occ = System_Info::Get_RAM_Info();
    QString str = QString("RAM: %1%  ").arg(mem_occ);
    lbl_RAM->setText(str);
    // int cpu = System_Info::Get_CPU_Info();//kadun
    // str = QString("CPU: %1%  ").arg(cpu,2,10,QChar(' '));
    // lbl_CPU->setText(str);
}
void MainWindow::Init_IMUReader()
{
    if (!_imuReader.InitIMU())
        printf("InitIMU error.\n");
    _imuReader.StopTrigger();
}

void MainWindow::Init_CameraParam()
{
    QStringList strList;
    strList << "On" << "Off";
    ui->comboBox_triMode->addItems(strList);
    strList.clear();
    strList << "Software" << "Line1";
    ui->comboBox_triSource->addItems(strList);
    ui->comboBox_triSource->setCurrentIndex(1);
    ui->comboBox_triMode->setEditable(false);
    ui->comboBox_triSource->setEditable(false);

    QDoubleValidator *expValidator = new QDoubleValidator;
    expValidator->setRange(16,15000);
    ui->lineEdit_exp->setValidator(expValidator);

    QDoubleValidator *gainValidator = new QDoubleValidator;
    gainValidator->setRange(1,6);
    ui->lineEdit_gain->setValidator(gainValidator);

    QDoubleValidator *gammaValidator = new QDoubleValidator;
    gammaValidator->setRange(0,3.99998);
    ui->lineEdit_gamma->setValidator(gammaValidator);
}

void MainWindow::Init_SerialPort()
{
    connect(&serial,SIGNAL(readyRead()),this,SLOT(serialRead()));
    serial.close();
    QString strpose_port = Ini_Reader::_pose_port;
    serial.setPortName(strpose_port);//"/dev/ttyUSB0"
    if(serial.open(QIODevice::ReadWrite)){          //读写打开
        serial.setBaudRate(QSerialPort::Baud38400); //波特率Baud38400
        serial.setDataBits(QSerialPort::Data8);     //数据位8
        serial.setParity(QSerialPort::OddParity);   //奇校验
        serial.setStopBits(QSerialPort::OneStop);   //停止位1
        serial.setFlowControl(QSerialPort::NoFlowControl);  //无控制
    }else{
        perror("Serialport Open Fail");
    }
    if(serial.isOpen()){
        printf("--------------SerialPort Open Sucess!--------------\n");
    }else{
        printf("--------------SerialPort Open Fail!--------------\n");
    }
#if 0
    while(1){//test...
        static char dat[29] = {(char)0x55,(char)0xaa,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6};
        dat[2]++;
        if(serial.isOpen()){
            qint64 cnt = serial.write(dat,29);
            // printf("cnt = %d \n",cnt);
            serial.flush();
        }
    }
#endif
}

void MainWindow::serialRead()
{
    // QByteArray qa = serial.readAll();
}

void MainWindow::on_pushButton_clicked()
{
    double exposureTime = ui->lineEdit_exp->text().toFloat();
    double gainRaw = ui->lineEdit_gain->text().toFloat();
    double gamma = ui->lineEdit_gamma->text().toFloat();
    manager->_cameras[0]->SetAdjustPlus(gainRaw);
    manager->_cameras[0]->SetExposeTime(exposureTime);
    manager->_cameras[0]->SetExposeTime(gamma);
    manager->_cameras[1]->SetAdjustPlus(gainRaw);
    manager->_cameras[1]->SetExposeTime(exposureTime);
    manager->_cameras[1]->SetExposeTime(gamma);
}

void MainWindow::start()
{
    stop();
    usleep(2000);//delay 2ms
    int ret = _imuReader.SendTriggerInfo();//udp:55 aa 01
    if(ret < 0){
        qDebug() << "Start fail,Please check the network!";//no use
    }
    usleep(10000);//10ms
    if(!_imuReader._bTrigger_Flag){
        //    QMessageBox::warning(this, tr("3VCam"), tr("Start fail,Please check the network!"));
    }
}

void MainWindow::stop()
{
    int ret = _imuReader.StopTrigger();//udp:55 aa 02
    if(ret < 0){
        qDebug() << "Stop fail,Please check the network!";
    }
}

//last_toolbar_button
void MainWindow::exit()
{       
    _imuReader.StopTrigger();
    if(manager)
        manager->Stop();
    close();
}

//menu
void MainWindow::on_actionExit_triggered()
{
    exit();
}

//about menu
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog dlg;
    dlg.exec();
}

//about toolbar
void MainWindow::about()
{
    aboutDialog dlg;
    dlg.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F12)//cal_store_pic TODO
    {
        QMessageBox::aboutQt(NULL, "aboutQt");
    }
}

void MainWindow::Init_ToolBar()
{
    startAction = new QAction(QIcon(":/res/Normal_Play"), tr("&Start"), this);
    ui->mainToolBar->addAction(startAction);
    connect(startAction, &QAction::triggered,this, &MainWindow::start);

    stopAction = new QAction(QIcon(":/res/Normal_Pause"), tr("&Stop"), this);
    ui->mainToolBar->addAction(stopAction);
    connect(stopAction, &QAction::triggered,this, &MainWindow::stop);
    ui->mainToolBar->addSeparator();

    testAction3 = new QAction(QIcon(":/res/icon3_Normal"), tr("&Power"), this);
    ui->mainToolBar->addAction(testAction3);
    testAction4 = new QAction(QIcon(":/res/icon4_Normal"), tr("&Tracking"), this);
    ui->mainToolBar->addAction(testAction4);
    testAction5 = new QAction(QIcon(":/res/icon5_Normal"), tr("&Reset"), this);
    ui->mainToolBar->addAction(testAction5);
    testAction6 = new QAction(QIcon(":/res/icon6_Normal"), tr("&OnAir"), this);
    ui->mainToolBar->addAction(testAction6);
    testAction7 = new QAction(QIcon(":/res/icon7_Normal"), tr("&Wireframe"), this);
    ui->mainToolBar->addAction(testAction7);
    ui->mainToolBar->addSeparator();

    //  rebootAction = new QAction(QIcon(":/res/Normal_Cal"), tr("&Reboot..."), this);
    //  ui->mainToolBar->addAction(rebootAction);
    //  connect(rebootAction, &QAction::triggered,this, &MainWindow::reboot);
    //about dialog
    aboutAction = new QAction(QIcon(":/res/Normal_About"), tr("&About"), this);
    ui->mainToolBar->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered,this, &MainWindow::about);
    //Exit button
    exitAction = new QAction(QIcon(":/res/Normal_Close"), tr("&Exit"), this);
    ui->mainToolBar->addAction(exitAction);
    connect(exitAction, &QAction::triggered,this, &MainWindow::exit);
    ui->mainToolBar->addSeparator();
    //add logo
    QLabel *blank = new QLabel(this);
    blank->setPixmap(QPixmap(":/res/blank"));
    ui->mainToolBar->addWidget(blank);
    QLabel *logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/res/logo"));
    ui->mainToolBar->addWidget(logo);

    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void MainWindow::Init_StatusBar()
{
    QLabel *lbl_Cam_01 = new QLabel;
    lbl_Cam_01->setText(tr("Tracking Camera01: "));
    lbl_Cam_01->setMinimumSize(lbl_Cam_01->sizeHint());
    lbl_Cam_01->setAlignment(Qt::AlignHCenter);
    lbl_Cam_01->setIndent(5);
    statusBar()->addWidget(lbl_Cam_01);
    lbl_HardWare_S1 = new QLabel;
    lbl_HardWare_S1->setPixmap(QPixmap(":/res/Wrong"));//Normal Wrong
    lbl_HardWare_S1->setIndent(130);
    statusBar()->addWidget(lbl_HardWare_S1);
    QLabel *lbl_blank1 = new QLabel;
    lbl_blank1->setText(tr("        "));
    statusBar()->addWidget(lbl_blank1);
    //Camera_02
    QLabel *lbl_Cam_02 = new QLabel;
    lbl_Cam_02->setText(tr("Tracking Camera02: "));
    statusBar()->addWidget(lbl_Cam_02);
    lbl_HardWare_S2 = new QLabel;
    lbl_HardWare_S2->setPixmap(QPixmap(":/res/Wrong"));
    statusBar()->addWidget(lbl_HardWare_S2);
    QLabel *lbl_blank2 = new QLabel;
    lbl_blank2->setText(tr("        "));
    statusBar()->addWidget(lbl_blank2);
    //FreeD
    QLabel *lbl_FreeD = new QLabel;
    lbl_FreeD->setText(tr("FreeD: "));
    statusBar()->addWidget(lbl_FreeD);
    lbl_HardWare_S3 = new QLabel;
    lbl_HardWare_S3->setPixmap(QPixmap(":/res/Normal"));
    statusBar()->addWidget(lbl_HardWare_S3);
    QLabel *lbl_blank3 = new QLabel;
    // lbl_blank3->setText(tr("        "));
    // statusBar()->addWidget(lbl_blank3);
    //  Main Camera
    //   QLabel *lbl_MainCamera = new QLabel;
    //   lbl_MainCamera->setText(tr("Main_Camera: "));
    //   statusBar()->addWidget(lbl_MainCamera);
    //   lbl_HardWare_S4 = new QLabel;
    //   lbl_HardWare_S4->setPixmap(QPixmap(":/res/Normal"));
    //   statusBar()->addWidget(lbl_HardWare_S4);
    QLabel *lbl_blank11 = new QLabel;
    lbl_blank11->setText(tr("                         "));
    statusBar()->addWidget(lbl_blank11);
    QLabel *lbl_blank4 = new QLabel;
    lbl_blank4->setText(tr("                                                                                                                                                    "));
    statusBar()->addWidget(lbl_blank4);
    //PC resources(CPU,GPU,RAM)
    //CPU
    lbl_CPU = new QLabel;
    lbl_CPU->setText(tr("CPU:10%     "));
    statusBar()->addWidget(lbl_CPU);
    lbl_GPU = new QLabel;
    lbl_GPU->setText(tr("GPU:10%     "));
    // statusBar()->addWidget(lbl_GPU);
    lbl_RAM = new QLabel;
    lbl_RAM->setText(tr("RAM:10%"));
    statusBar()->addWidget(lbl_RAM);
    statusBar()->setSizeGripEnabled(true); //设置是否显示右边的大小控制点
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    exit();
}
