#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <QtCore>
#include <QtOpenGL>
#include <QtOpenGL/qglshaderprogram.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <QMutex>

#include <osAbstraction/Include/osAbstraction.h>

using namespace std;
using namespace mv;

//-----------------------------------------------------------------------------
class GLESWidget : public QGLWidget
//-----------------------------------------------------------------------------
{
    Q_OBJECT

    unsigned int           iWidth;
    unsigned int           iHeight;
    float                  fWidth;
    float                  fHeight;
    QGLShaderProgram       ShaderProgram_;
    GLuint                 texId;
    int                    frameNr;
    GLuint                 baseMapTexId;
    bool                   AcquisitionStarted;
    QMutex                 VideoSyncMutex;
    bool                   IsInitialised;
    bool                   VideoSrcIsYUV;
    bool                   TextureLoaded;
    
    char* LoadShader( std::string filename );
    
public:
    GLESWidget(unsigned int w, unsigned int h, QWidget *parent = 0);
    ~GLESWidget();

    void VideoEnable( bool enable );
    void VideoSyncMutexLock( void )	{ VideoSyncMutex.lock(); }
    void VideoSyncMutexUnlock( void ) { VideoSyncMutex.unlock(); }
    void ForceUnlockVideoSyncMutex(void) { VideoSyncMutex.unlock(); }
    void SetVideoSrcIsYUV( bool mode ) { VideoSrcIsYUV = mode; }

protected:
    void initializeGL();
    void paintGL();
    void DoPaint(void);

public slots:
    void DoLoadTexture( unsigned char* inbuffer );
    void DoUpdateGL( int reqNr );
};

#endif // GLESWIDGET_H
