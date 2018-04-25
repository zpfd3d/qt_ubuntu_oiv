#include "ini_reader.h"
#include <QSettings>
#include <QDebug>

//init
int Ini_Reader::_fps(0);
QString Ini_Reader::_left_sn("");
QString Ini_Reader::_right_sn("");
int Ini_Reader::_stretching(0);
QString Ini_Reader::_pose_port("");
QString Ini_Reader::_encoder_port("");
int Ini_Reader::_Capture_Card_Port(0);
int Ini_Reader::_Video_Standard(0);

void  Ini_Reader::Ini_Read()
{
    //write test
#if 0
    QSettings configIniWrite("config/config.ini", QSettings::IniFormat);
    configIniWrite.setValue("/Camera/fps", 30);
    configIniWrite.setValue("/Camera/left_sn", "3G0172AAAK00001");
    configIniWrite.setValue("/Camera/right_sn", "3G0172AAAK00008");

    configIniWrite.setValue("/SerialPort/pose_port", "ttyUSB0");
    configIniWrite.setValue("/SerialPort/encoder_port", "ttyUSB1");

    configIniWrite.setValue("/Capture_Card/Capture_Card_Port", 1);
    configIniWrite.setValue("/Capture_Card/Video_Standard", 4);
#endif
    //Read
    QSettings configIniRead("config/config.ini", QSettings::IniFormat);
    _fps        = configIniRead.value("/Camera/fps").toInt();
    _left_sn    = configIniRead.value("/Camera/left_sn").toString();
    _right_sn   = configIniRead.value("/Camera/right_sn").toString();
    _stretching = configIniRead.value("/Camera/stretching").toInt();

    _pose_port    = configIniRead.value("/SerialPort/pose_port").toString();
    _encoder_port = configIniRead.value("/SerialPort/encoder_port").toString();

    _Capture_Card_Port = configIniRead.value("/Capture_Card/Capture_Card_Port").toInt();
    _Video_Standard    = configIniRead.value("/Capture_Card/Video_Standard").toInt();

    //Print the result
    qDebug() << "fps = " << _fps;
    qDebug() << "_left_sn = " << _left_sn;
    qDebug() << "_right_sn = " << _right_sn;
    qDebug() << "_stretching = " << _stretching;
    qDebug() << "_pose_port = " << _pose_port;
    qDebug() << "_encoder_port = " << _encoder_port;
    qDebug() << "_Capture_Card_Port = " << _Capture_Card_Port;
    qDebug() << "_Video_Standard = " << _Video_Standard;
}
