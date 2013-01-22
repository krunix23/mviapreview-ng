#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QtCore>
#include <osAbstraction/Include/osAbstraction.h>
#include <gleswidget.h>

#include <mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

using namespace std;
using namespace mv;
using namespace mvIMPACT::acquire;

//-----------------------------------------------------------------------------
class WorkerThread : public QThread
//-----------------------------------------------------------------------------
{
    Q_OBJECT

    void run(void);

	DeviceManager*         pDevMgr;
    Device*                pDev;
    unsigned int           iWidth;
    unsigned int           iHeight;
    bool                   DoRunThread;
    GLESWidget*            pGL;
    FunctionInterface*     pFI;
    bool                   IsColorSensor;
    int                    frameNr;
    
    void OpenBlueLYNXDevice(void);
    void OpenBlueFOXDevice(void);
    void OpenVirtualDevice(void);
    void StartAcquisition( void );
    void StopAcquisition( void );
    int DequeueVideo( void );
    void ReleaseRequest(int requestNr);

signals:
    void DoLoadTexture( unsigned char* inbuffer );
    void DoUpdateGL( int reqNr );
    void DoUpdateStatusBar( QString message );

public:
    WorkerThread( DeviceManager *pDevMgr_, QObject* parent = 0 );
    void AttachGLWidget( GLESWidget* pGL_ ) { pGL = pGL_; }
    void OpenDevice(void);
    void StartThread( void );
    void StopThread( void );
    
public slots:
    void DoWhiteBalanceIA(bool arg);
    void DoAutoExposureIA(bool arg);
};

#endif // WORKERTHREAD_H
