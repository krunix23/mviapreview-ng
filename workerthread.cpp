#include "workerthread.h"

//-----------------------------------------------------------------------------
WorkerThread::WorkerThread( DeviceManager *pDevMgr_, QObject* parent ) : QThread(parent),
pDevMgr(pDevMgr_), pDev(0), DoRunThread(false), frameNr(0)
//-----------------------------------------------------------------------------
{
	iWidth = 640;
	iHeight = 480;
}

//-----------------------------------------------------------------------------
void WorkerThread::OpenDevice( unsigned int w, unsigned int h )
//-----------------------------------------------------------------------------
{
	iWidth = w;
	iHeight = h;
	
#if defined(ARM)
    OpenBlueLYNXDevice();
    //OpenVirtualDevice();
#else
    OpenBlueFOXDevice();
    //OpenVirtualDevice();
#endif
}

//-----------------------------------------------------------------------------
void WorkerThread::OpenBlueLYNXDevice(void)
//-----------------------------------------------------------------------------
{
    pDev = pDevMgr->getDeviceBySerial(std::string("LX*"));

    if(pDev)
    {
        pDev->interfaceLayout.write( dilGenICam );
        pDev->open();

        SystemSettings sys(pDev);
        sys.requestCount.write(16);

        pFI = new FunctionInterface(pDev);

        mvIMPACT::acquire::GenICam::AcquisitionControl ac(pDev);
        mvIMPACT::acquire::GenICam::ImageFormatControl ifc(pDev);
        mvIMPACT::acquire::GenICam::CounterAndTimerControl ctc(pDev);

        int maxw = ifc.widthMax.read();
        int maxh = ifc.heightMax.read();

        int xoff = (maxw - iWidth) / 2;
        int yoff = (maxh - iHeight) / 2;

        if( xoff < 0 )
            xoff = 0;
        if( yoff < 0 )
            yoff = 0;

        ifc.width.write( iWidth );
        ifc.height.write( iHeight );
        ifc.offsetX.write( xoff );
        ifc.offsetY.write( yoff );
        try
        {
        	ifc.pixelFormat.writeS( "YUV422Packed" );
        	IsColorSensor = true; // if it's not, an exception is thrown when writing pixelFormat=YUV422Packed
        }
        catch( ... )
        {
        	ImageDestination id(pDev);
        	id.pixelFormat.writeS("YUV422_UYVYPacked");
        	IsColorSensor = false;
        }

        ac.acquisitionMode.writeS( "Continuous" );
        ac.exposureTime.write(2000.0);

        ctc.timerSelector.writeS( "Timer1" );
        ctc.timerTriggerSource.writeS( "Timer1End" );
        ctc.timerDuration.write( 83333.0 ); // 12Hz = 83333.0

        ac.triggerSource.writeS( "Timer1End" );
        ac.triggerMode.writeS( "On" );
        
        pGL->SetVideoSrcIsYUV(true);
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::OpenBlueFOXDevice(void)
//-----------------------------------------------------------------------------
{
    pDev = pDevMgr->getDeviceBySerial(std::string("BF*"));

    if(pDev)
    {
        pDev->open();

        pFI = new FunctionInterface(pDev);

        SystemSettings sys(pDev);
        CameraSettingsBase cs(pDev);
        CameraSettingsBlueFOX csbf(pDev);
        ImageDestination id(pDev);

        int maxw = cs.aoiWidth.getMaxValue();
        int maxh = cs.aoiHeight.getMaxValue();

        int xoff = (maxw - iWidth) / 2;
        int yoff = (maxh - iHeight) / 2;

        if( xoff < 0 )
            xoff = 0;
        if( yoff < 0 )
            yoff = 0;

        cs.aoiWidth.write(iWidth);
        cs.aoiHeight.write(iHeight);
        cs.aoiStartX.write(xoff);
        cs.aoiStartY.write(yoff);
        sys.requestCount.write(16);
        csbf.pixelClock_KHz.write(cpc40000KHz);
        csbf.expose_us.write(20000);
        id.pixelFormat.write(idpfRGBx888Packed);
        
        pGL->SetVideoSrcIsYUV(false);
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::OpenVirtualDevice(void)
//-----------------------------------------------------------------------------
{
    pDev = pDevMgr->getDeviceBySerial(std::string("VD000001"));

    if(pDev)
    {
        pDev->open();

        pFI = new FunctionInterface(pDev);

        SystemSettings sys(pDev);
        CameraSettingsVirtualDevice csvd(pDev);
        CameraSettingsBase cs(pDev);
        ImageDestination id(pDev);

        cs.aoiWidth.write(iWidth);
        cs.aoiHeight.write(iHeight);
        cs.aoiStartX.write(0);
        cs.aoiStartY.write(0);
        sys.requestCount.write(16);

        #ifdef ARM
        csvd.frameDelay_us.write(40000);
        #else
        csvd.frameDelay_us.write(10000);
        #endif

        csvd.testMode.write(vdtmMovingBayerDataRamp);
        id.pixelFormat.write(idpfRGBx888Packed);
        
        pGL->SetVideoSrcIsYUV(false);
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::StartThread()
//-----------------------------------------------------------------------------
{
    if( pDev && !isRunning() && !DoRunThread )
    {
        DoRunThread = true;
        pGL->VideoEnable(true);
        start();
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::StopThread()
//-----------------------------------------------------------------------------
{
    DoRunThread = false;
    pGL->VideoEnable(false);
    msleep(100);// give thread a chance to exit ordinary
    if(isRunning())
    {
        pGL->ForceUnlockVideoSyncMutex();
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::run(void)
//-----------------------------------------------------------------------------
{
	unsigned char* buf = NULL;
    int requestNr;
    const Request* pRequest = 0;
    unsigned char cnt = 0;
    StartAcquisition();

    while( DoRunThread )
    {
	    cnt++;
	    pGL->VideoSyncMutexLock();
        requestNr = DequeueVideo();
        if( pFI->isRequestNrValid( requestNr ) )
        {
            pRequest = pFI->getRequest( requestNr );
            if( pRequest->isOK() )
            {
	            buf = (unsigned char*)pRequest->imageData.read();
	            
	            emit DoLoadTexture( buf );
	            
	            frameNr = pRequest->infoFrameNr.read();
	            
	            if( frameNr % 10 == 0)
                {
                    Statistics ss(pDev);
                    char buf[16];
                    sprintf(buf, "%3.1f", ss.framesPerSecond.read() );

                    QString message;
                    message += QString(buf);
                    message += " fps - ";
                    message += pRequest->imageWidth.readS().c_str();
                    message += "x";
                    message += pRequest->imageHeight.readS().c_str();
                    message += " - frame#: ";
                    message += pRequest->infoFrameNr.readS().c_str();
                    emit DoUpdateStatusBar(message);
                }
            }
        	emit DoUpdateGL( requestNr );            
        }
        ReleaseRequest(requestNr);
    }
    
    StopAcquisition();
    DoRunThread = false;
    exit(0);
}

//-----------------------------------------------------------------------------
void WorkerThread::StartAcquisition( void )
//-----------------------------------------------------------------------------
{
    int numRequests = 0;
    int requestResult = DMR_NO_ERROR;

    while( ( requestResult = pFI->imageRequestSingle() ) == DMR_NO_ERROR )
    {
        numRequests++;
    }
    printf("Starting videostream and using %d buffers\n", numRequests );
    pFI->acquisitionStart();
}

//-----------------------------------------------------------------------------
void WorkerThread::StopAcquisition( void )
//-----------------------------------------------------------------------------
{
    pFI->acquisitionStop();
    pFI->imageRequestReset(0,0);
    pGL->update();
}

//-----------------------------------------------------------------------------
int WorkerThread::DequeueVideo( void )
//-----------------------------------------------------------------------------
{
    if(!pDev || !pFI )
        return -1;

    return pFI->imageRequestWaitFor(2000);
}

//-----------------------------------------------------------------------------
void WorkerThread::ReleaseRequest(int requestNr)
//-----------------------------------------------------------------------------
{
    pGL->VideoSyncMutexLock();
    pFI->imageRequestUnlock(requestNr);
    pFI->imageRequestSingle();
    pGL->VideoSyncMutexUnlock();
}

//-----------------------------------------------------------------------------
void WorkerThread::DoWhiteBalanceIA(bool arg)
//-----------------------------------------------------------------------------
{
    if( !std::string("mvBlueLYNX").compare( pDev->family.readS() ) && IsColorSensor )
    {
		mvIMPACT::acquire::GenICam::mvOMAPPreviewConfig pc(pDev);

        if( arg )
            pc.mvBalanceWhiteAuto.writeS("Continuous");
        else
            pc.mvBalanceWhiteAuto.writeS("Off"); // 2 | Off
    }
}

//-----------------------------------------------------------------------------
void WorkerThread::DoAutoExposureIA(bool arg)
//-----------------------------------------------------------------------------
{
    if( !std::string("mvBlueLYNX").compare( pDev->family.readS() ) && IsColorSensor )
    {
        mvIMPACT::acquire::GenICam::mvOMAPPreviewConfig pc(pDev);

        if( arg )
            pc.mvAutoExposure.writeS("Continuous");
        else
            pc.mvAutoExposure.writeS("Off");
    }
}

