/*
    读取IMU
*/

#ifndef __IMUReader_H__
#define __IMUReader_H__

#include <arpa/inet.h>
#include <sys/socket.h>

class IMUReader
{
public:
    IMUReader();
    ~IMUReader();

    bool InitIMU();
    int  SendTriggerInfo();
    bool StopTrigger();    
public:
    int     _imu_sock;
    struct  sockaddr_in _addr_mcu;
    bool    _bTrigger_Flag;
private:
    int    _fps;
    struct sockaddr_in _addr_pc;
    const int _PC_PORT;
    const int _MCU_PORT;
};

#endif // __IMUReader_H__
