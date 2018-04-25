#include <QString>
#include <QObject>

#ifndef INI_READER_H
#define INI_READER_H

class Ini_Reader : public QObject
{
public:
    //camera parameters
    static int _fps;          // 30 50 60...
    static QString _left_sn;  //left camera sn
    static QString _right_sn; //right camera sn
    static int _stretching;   //right camera sn
    //serial_port parameters
    static QString _pose_port;    //x,y,z and pitch, yaw, roll
    static QString _encoder_port; //zoom, focus(may no need)
    //capture_card parameters
    static int _Capture_Card_Port;
    static int _Video_Standard;   //N P
public:
    static void Ini_Read();
};

#endif // INI_READER_H
