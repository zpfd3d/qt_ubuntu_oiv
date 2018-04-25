#ifndef CAMERAWND_H
#define CAMERAWND_H

#include <QWidget>
#include "GenICam/Camera.h"
#include "GenICam/System.h"
#include "Media/VideoRender.h"
#include "Media/ImageConvert.h"
#include "messageque.h"

#include <math.h>

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

class CFrameInfo : public Dahua::Memory::CBlock
{
public:
    CFrameInfo()
    {
        m_pImageBuf = NULL;
        m_nBufferSize = 0;
        m_nWidth = 0;
        m_nHeight = 0;
        m_PixelType = Dahua::GenICam::gvspPixelMono8;
        m_nPaddingX = 0;
        m_nPaddingY = 0;
        m_nTimeStamp = 0;
    }

    ~CFrameInfo()
    {
    }

public:
    BYTE		*m_pImageBuf;
    int			m_nBufferSize;
    int			m_nWidth;
    int			m_nHeight;
    Dahua::GenICam::EPixelType	m_PixelType;
    int			m_nPaddingX;
    int			m_nPaddingY;
    uint64_t	m_nTimeStamp;
};

namespace Ui {
class CameraWnd;
}

class CameraWnd : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWnd(QWidget *parent = 0);
    ~CameraWnd();

    bool _bLeft;
    enum ETrigType
    {
        trigContinous = 0,	//连续拉流
        trigSoftware = 1,	//软件触发
        trigLine = 2,		//外部触发
    };

    void SetCamera(ICameraPtr camera);

    void OnGetFrameAnsy(const CFrame & frame);
    bool Connect();
    bool DisConnect();
    bool Play();
    bool StopPlay();
    bool SaveConfig();
    void CameraChangeTrig(ETrigType trigType);
    bool SetAdjustPlus(double gainRaw);
    bool SetExposeTime(double exposureTime);
    // 显示线程
    void DisplayThreadProc(Dahua::Infra::CThreadLite& lite);
    bool ShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t pixelFormat);
private:
    bool SetLineTriggerMode();
private:
    Ui::CameraWnd *ui;

    ICameraPtr m_pCamera;
    IStreamSourcePtr m_pStreamSource;
    //CRender                             _render;          // 显示对象
    Dahua::Infra::CThreadLite           _displayThread;   // 显示线程
    TMessageQue<CFrameInfo>				_displayFrameQueue;// 显示队列

    Dahua::Infra::CMutex				m_mxTime;
    double								m_dDisplayInterval;         // 显示间隔
    uintmax_t							m_nTimestampFreq;           // 时间戳频率
    uintmax_t							m_nFirstFrameTime;          // 第一帧的时间戳
    uintmax_t							m_nLastFrameTime;           // 上一帧的时间戳

    VR_HANDLE          m_handler;           /* 绘图句柄 */
    VR_OPEN_PARAM_S    m_params;            /* 显示参数 */
    // 计算该帧是否显示
    bool isTimeToDisplay(uintmax_t nCurTime);
    /* 显示相关函数 */
    bool open(uint32_t width, uint32_t height);
    bool close();

    void closeEvent(QCloseEvent * event);

    VR_HWND		m_hWnd;

};

#endif // CAMERAWND_H
