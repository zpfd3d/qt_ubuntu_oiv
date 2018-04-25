#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QQueue>
#include "cameramanager.h"
#include "ini_reader.h"
#include "IMUReader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);    
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_clicked();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void serialRead();
    void timerUpDateSysInfo();
    void timerShowPoseData();
public:
    QTimer *timerPoseData;
    QTimer *timerGetSystemInfo;
    IMUReader _imuReader;
    CameraManager* manager;
public:
    void Init_ToolBar();
    void Init_StatusBar();
    void Init_CameraParam();
    void Init_SerialPort();
    void Init_IMUReader();
    void Init_Timer();
    void Camera_Connect();
private:
    Ui::MainWindow *ui;
    //Toolbar
    QAction *startAction;
    QAction *stopAction;
    QAction *testAction3;
    QAction *testAction4;
    QAction *testAction5;
    QAction *testAction6;
    QAction *testAction7;
    QAction *aboutAction;
    QAction *exitAction;
    void start();
    void stop();
    void about();
    void exit();
    //Statusbar
    QLabel *lbl_HardWare_S1;//camera 01 status
    QLabel *lbl_HardWare_S2;//camera 02 status
    QLabel *lbl_HardWare_S3;//FreeD status
    QLabel *lbl_HardWare_S4;//MainCamera status

    QLabel *lbl_CPU;
    QLabel *lbl_GPU;
    QLabel *lbl_RAM;
};

#endif // MAINWINDOW_H
