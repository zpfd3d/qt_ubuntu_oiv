#include "cameramanager.h"
#include "ini_reader.h"
CameraManager::CameraManager():_parent(NULL)
{

}

CameraManager::~CameraManager()
{

}

int CameraManager::Start()
{
    bool bRet = false;
    int ret = 0;
    if(FindDevice())
    {
        size_t iShowCameraNum = _vecCameras.size();
        //    _cameras = new CameraWnd[iShowCameraNum];
        for(size_t iIndex = 0; iIndex < iShowCameraNum; iIndex++)
        {
            _cameras[iIndex] = new CameraWnd(_parent);
            _cameras[iIndex]->setParent(_parent);
            _cameras[iIndex]->SetCamera(_vecCameras[iIndex]);
            bRet = _cameras[iIndex]->Connect();
            if(bRet && (0==iIndex)){
                ret += 0x01;
            }else if(bRet && (1==iIndex)){
                ret += 0x02;
            }
            _cameras[iIndex]->CameraChangeTrig(CameraWnd::trigLine);
            _cameras[iIndex]->Play();

        }
        return ret;
    }
    return ret;

}

bool CameraManager::Stop()
{
    size_t iShowCameraNum = _vecCameras.size();
    for(size_t iIndex = 0; iIndex < iShowCameraNum; iIndex++)
    {
        _cameras[iIndex]->StopPlay();
        _cameras[iIndex]->DisConnect();
    }
}

void CameraManager::SetParent(QWidget *parent)
{
    _parent = parent;
}

bool CameraManager::FindDevice()
{
    CSystem& _systemIns = CSystem::getInstance();
    bool isDiscoverSuccess = _systemIns.discovery(_vecCameras);

    size_t iShowCameraNum = _vecCameras.size();

    if(iShowCameraNum == 2)
        return true;
    return false;
}

void CameraManager::MoveWindow(const QRect& rt)
{
    size_t iShowCameraNum = _vecCameras.size();
    int width = (rt.right() - rt.left()) /2 ;
    int stretching = Ini_Reader::_stretching;
    int height;
    if(1 == stretching){
        height = (rt.bottom() - rt.top());
    }else{
        height = 5 * width /8;
    }
    int offset = (rt.height() - height)/2;
    for(size_t iIndex = 0; iIndex < iShowCameraNum; iIndex++)
    {
        if(_cameras[iIndex]->_bLeft)
        {
            _cameras[iIndex]->setGeometry(0 * width,rt.y()+offset, width ,height);
        }
        else
        {
            _cameras[iIndex]->setGeometry(1 * width+1,rt.y()+offset, width ,height);
        }
        _cameras[iIndex]->show();
    }
}
