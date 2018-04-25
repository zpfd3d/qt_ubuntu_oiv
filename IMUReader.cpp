/*
    读取IMU
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "IMUReader.h"
#include "ini_reader.h"

void* UdpRxThread(void* param);
IMUReader::IMUReader():_PC_PORT(6666),_MCU_PORT(8888)
{
    _bTrigger_Flag = false;
    _fps = 50;
    _imu_sock = -1;
    memset(&_addr_pc, 0, sizeof(_addr_pc));
    memset(&_addr_mcu, 0, sizeof(_addr_mcu));
}

IMUReader::~IMUReader()
{
    if(_imu_sock > 0)
        close(_imu_sock);
    _imu_sock = -1;
}

bool IMUReader::InitIMU()
{
    _fps = Ini_Reader::_fps;
    printf("InitIMU() : fps = %d\n",_fps);
    memset(&_addr_pc, 0, sizeof(_addr_pc));
    _addr_pc.sin_family = AF_INET;
    _addr_pc.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr_pc.sin_port = htons(_PC_PORT);

    _imu_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (_imu_sock == -1)
    {
        printf("IMUReader::InitIMU error.\n");
        return false;
    }
    int on = 1;
    setsockopt(_imu_sock,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
    if(-1 == ( bind( _imu_sock,(struct sockaddr*)&_addr_pc, sizeof(_addr_pc))))
    {
        printf("bind error\n");
        perror("bind error");
        exit (1);
    }

    bzero(&_addr_mcu,sizeof(_addr_mcu));
    _addr_mcu.sin_family=AF_INET;
    char ip[30] = {0};
    strcpy(ip, "192.168.1.200");//mcu ip
    _addr_mcu.sin_addr.s_addr= inet_addr(ip);
    _addr_mcu.sin_port=htons(_MCU_PORT);//mcu port(8888)
    //Create IMU Receive Thread
    pthread_t thread_id;
    pthread_create(&thread_id,NULL, UdpRxThread, this);
    return true;
}

#define CD_GRAVITY 9.7913
void* UdpRxThread(void* param)
{
    IMUReader *reader = (IMUReader *)param;
    unsigned short timestamp;
    short acc_x, acc_y, acc_z;//加速度三轴原始数据
    short gyr_x, gyr_y, gyr_z;//陀螺仪三轴原始数据
    float imu[6] = {0.0f}; //加速度，陀螺仪 6个处理后的数据. 度每秒
    const static int RX_LEN = 1000;
    char buf_imu[RX_LEN] = {0};
    socklen_t len = sizeof(reader->_addr_mcu);
    while (1) {
        int n = recvfrom(reader->_imu_sock, buf_imu, RX_LEN, 0, (sockaddr *)&(reader->_addr_mcu), &len);
        if (n > 0){
            if (buf_imu[0] == (char)0x55 && buf_imu[1] == (char)0xAA && buf_imu[18] == (char)0x88 && buf_imu[19] == (char)0xFF) {//包头包尾验证
                timestamp = (unsigned char)buf_imu[2] | (unsigned char)buf_imu[3] << 8;
                gyr_x = (unsigned char)buf_imu[4]  |  (unsigned char)buf_imu[5] << 8;
                gyr_y = (unsigned char)buf_imu[6]  |  (unsigned char)buf_imu[7] << 8;
                gyr_z = (unsigned char)buf_imu[8]  |  (unsigned char)buf_imu[9] << 8;
                acc_x = (unsigned char)buf_imu[10] |  (unsigned char)buf_imu[11] << 8;
                acc_y = (unsigned char)buf_imu[12] |  (unsigned char)buf_imu[13] << 8;
                acc_z = (unsigned char)buf_imu[14] |  (unsigned char)buf_imu[15] << 8;
                imu[0] = acc_x / 1200.0 * CD_GRAVITY;
                imu[1] = acc_y / 1200.0 * CD_GRAVITY;
                imu[2] = acc_z / 1200.0 * CD_GRAVITY;
                imu[3] = gyr_x / 25.0;
                imu[4] = gyr_y / 25.0;
                imu[5] = gyr_z / 25.0;
                //	printf("timestamp = %d,acc= %d,%d,%d gyr =  %d,%d,%d\n", timestamp, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z);
                printf("timestamp = %d,acc = %.3f,%.3f,%.3f,gyr =  %.3f,%.3f,%.3f\n", timestamp, imu[0], imu[1], imu[2], imu[3], imu[4], imu[5]);
            }
            reader->_bTrigger_Flag = true;
            break;//get only once(check if send sucessed).
        }
    }
}

int IMUReader::SendTriggerInfo()
{
    int ret(0);
    if (_imu_sock == -1)
        return ret;
    if (_fps == 30)
    {
        char imu_cmd[3] = { (char)0x55, (char)0xaa, 0x13};
        int addr_len = sizeof(_addr_pc);
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);

        imu_cmd[2] = 0x01;
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);
    }
    else if (_fps == 50)
    {
        char imu_cmd[3] = { (char)0x55, (char)0xaa, 0x14 };
        int addr_len = sizeof(_addr_pc);
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);

        imu_cmd[2] = 0x01;
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);
    }
    else if (_fps == 60)
    {
        char imu_cmd[3] = { (char)0x55, (char)0xaa, 0x15 };
        int addr_len = sizeof(_addr_pc);
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);

        imu_cmd[2] = 0x01;
        ret = sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);
    }
    return ret;
}

bool IMUReader::StopTrigger()
{
    if (_imu_sock == -1)
        return false;
    char imu_cmd[3] = { (char)0x55, (char)0xaa, 0x02 };
    int addr_len = sizeof(struct sockaddr_in);
    sendto(_imu_sock, imu_cmd, 3, 0, (struct sockaddr*)&_addr_mcu, addr_len);
    _bTrigger_Flag = false;
    return true;
}
