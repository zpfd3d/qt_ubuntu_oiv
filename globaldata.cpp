#include "globaldata.h"
#include "vislam_core.h"
#include <sys/time.h>
#include <QSerialPort>
#include <QSerialPortInfo>
extern QSerialPort serial;

GlobalData* GlobalData::m_pInstance = nullptr;

GlobalData::GlobalData()
{
    pthread_t thread_id;
    pthread_create(&thread_id,NULL, GlobalData::doWorkThread, this);
}
float gPan(0),gTile(0),gX(0),gY(0),gZ(0);
bool gFlag_PoseDataReady = false;
//#define _DEBUG_LY 1
#define _UART
void GlobalData::run()
{
    StereoImageBuffer imageBuffer;
    long lastTime = 0;
    unsigned char data[29] = { 0xD1 ,0xCA ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x08 ,0x00 ,0x00 ,0x08 ,0x00 ,0x00 ,0x00 ,0x00 ,0x95 };

    while(true)
    {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        long	time = tv.tv_sec*1000 + tv.tv_usec/1000;
        if(time - lastTime >= 12)
        {
            lastTime = time;
#ifdef _UART
            qint64 cnt = serial.write((const char*)data,29);
            serial.flush();
#endif
        }

        int lSize = _leftImageQue.size();
        int rSize = _rightImageQue.size();
        if(lSize > 0 && rSize > 0)
        {
            //printf("lSize = %d, rSize = %d\n",lSize, rSize);
            Frame* left = GlobalData::GetInstance()->_leftImageQue.front();
            Frame* right = GlobalData::GetInstance()->_rightImageQue.front();

            {
                //printf("left blockID = %d, right blockID= %d\n",left->blockID(), right->blockID());
                imageBuffer.leftImage = left->bufPtr();
                imageBuffer.rightImage = right->bufPtr();
                imageBuffer.width = left->imageWidth();
                imageBuffer.height = left->imageHeight();
                imageBuffer.timestamp = left->timeStamp();

                PoseData pose;
                float filter[2] = {0.3f, 0.3f};
            //    UpdateImage(&imageBuffer,&pose);//ALG

                //char timeLog[256] = { 0 };
                //showUpdateImageTime(timeLog);
                //printf("time:%s\n",timeLog);

                static int index = 0;
                char lName[256] = { 0 };
                char rName[256] = { 0 };
                index++;
                sprintf(lName,"l/%06d.png",index);
                sprintf(rName,"r/%06d.png",index);

                //ShowImage(&imageBuffer);
                //SaveImage(&imageBuffer,lName,rName);

                float d[10] = { pose.eulerAngle[0],pose.eulerAngle[1],pose.eulerAngle[2],pose.translation[0],pose.translation[1],pose.translation[2],
                                pose.orientation[0],pose.orientation[1],pose.orientation[2],pose.orientation[3] };

                //printf("-------%f,%f,%f,%f,%f,%f------\n",d[2],d[1],d[0],d[3],d[4],d[5]);
                //pan
                data[5] = (unsigned int)(d[2] * 32768) >> 16;
                data[6] = (unsigned int)(d[2] * 32768) >> 8;
                data[7] = (unsigned int)(d[2] * 32768);
                gPan = d[1];
                //Tile
                data[2] = (unsigned int)(d[1] * 32768) >> 16;
                data[3] = (unsigned int)(d[1] * 32768) >> 8;
                data[4] = (unsigned int)(d[1] * 32768);
                gTile = d[2];
                //Roll
                data[8] = (unsigned int)(d[0] * 32768) >> 16;
                data[9] = (unsigned int)(d[0] * 32768) >> 8;
                data[10] = (unsigned int)(d[0] * 32768);

                //x
                data[11] = (unsigned int)(d[3] * 1000 * 64) >> 16;
                data[12] = (unsigned int)(d[3] * 1000 * 64) >> 8;
                data[13] = (unsigned int)(d[3] * 1000 * 64);\

#ifdef  _DEBUG_LY
                static int cnt = 0;
                cnt++;
                gX = cnt;
#else
                gX = d[3];
#endif
                gY = d[4];
                gZ = d[5];
                //y
                data[17] = (unsigned int)(d[4] * 1000 * 64) >> 16;
                data[18] = (unsigned int)(d[4] * 1000 * 64) >> 8;
                data[19] = (unsigned int)(d[4] * 1000 * 64);

                //z
                data[14] = (unsigned int)(d[5] * 1000 * 64) >> 16;
                data[15] = (unsigned int)(d[5] * 1000 * 64) >> 8;
                data[16] = (unsigned int)(d[5] * 1000 * 64);
                gFlag_PoseDataReady = true;
                //ŒÆËãÐ£ÑéºÍ
                unsigned char bCSValue = 0x40;
                for (int i = 0; i < 28; i++)
                    bCSValue -= data[i];
                data[28] = bCSValue;

                #ifdef _UART
                    serial.write((const char*)data,29);
                    serial.flush();
                #endif
                struct timeval tv;
                gettimeofday(&tv,NULL);
                lastTime = tv.tv_sec*1000 + tv.tv_usec/1000;

                GlobalData::GetInstance()->_leftImageQue.pop();
                GlobalData::GetInstance()->_rightImageQue.pop();

                delete left;
                delete right;
            }

        }
    }
}
