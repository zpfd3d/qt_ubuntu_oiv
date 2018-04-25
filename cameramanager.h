#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

/*
 *This is Camera Manager Class
*/
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "Infra/Vector.h"
#include "camerawnd.h"
#include <QWidget>

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

class CameraManager
{
public:
    CameraManager();
    ~CameraManager();

    int  Start();
    bool Stop();

    void SetParent(QWidget* parent);
    void MoveWindow(const QRect& rt);

private:
    bool FindDevice();

public:
    TVector<ICameraPtr> _vecCameras;
    CameraWnd*          _cameras[2];
    QWidget*            _parent;
    bool                _bLeft;
};

#endif // CAMERAMANAGER_H
