#include "gleswidget.h"

using namespace mv;

//-----------------------------------------------------------------------------
GLESWidget::GLESWidget(unsigned int w, unsigned int h, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DirectRendering), parent),
    frameNr(0), AcquisitionStarted(false), IsInitialised(false), VideoSrcIsYUV(false)
//-----------------------------------------------------------------------------
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    makeCurrent();

    iWidth = w;
    iHeight = h;
    fWidth = static_cast<float>(iWidth);
    fHeight = static_cast<float>(iHeight);

    setFixedSize(w, h);
}

//-----------------------------------------------------------------------------
GLESWidget::~GLESWidget()
//-----------------------------------------------------------------------------
{
    glDeleteTextures ( 1, &texId );
}

//-----------------------------------------------------------------------------
char* GLESWidget::LoadShader( std::string filename )
//-----------------------------------------------------------------------------
{
        char* buffer = NULL;
        FILE* pFile = NULL;
        long lSize = 0;

        pFile = fopen( filename.c_str(), "rb" );
        if(pFile == NULL)
                return NULL;

        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);

        buffer = (char*)malloc( sizeof(char)*lSize );

        if(fread(buffer, 1, lSize, pFile) != (unsigned)lSize)
        {
                free(buffer);
                return NULL;
        }

        fclose(pFile);
        return buffer;
}

//-----------------------------------------------------------------------------
void GLESWidget::DoLoadTexture( unsigned char* inbuffer )
//-----------------------------------------------------------------------------
{
    unsigned char* buffer = NULL;

    if(!inbuffer || !AcquisitionStarted )
        return;
    
    makeCurrent();

	buffer = inbuffer;
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	
	glGenTextures ( 1, &texId );
	glBindTexture ( GL_TEXTURE_2D, texId );
	
	if( VideoSrcIsYUV )
	{
		glTexImage2D ( GL_TEXTURE_2D, 0,GL_LUMINANCE_ALPHA, iWidth, iHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer );
	}
	else
	{
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	}
	
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    doneCurrent();
}

//-----------------------------------------------------------------------------
void GLESWidget::DoUpdateGL( int /*reqNr*/ )
//-----------------------------------------------------------------------------
{
    updateGL();
}

//-----------------------------------------------------------------------------
void GLESWidget::initializeGL()
//-----------------------------------------------------------------------------
{
    QGLShader *vshader1 = new QGLShader(QGLShader::Vertex, this);
    const char *vsrc1 =
        "attribute vec4 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "gl_Position = a_position;\n"
        "v_texCoord = a_texCoord;\n"
        "}\n";
    vshader1->compileSourceCode(vsrc1);

    QGLShader *fshader1 = new QGLShader(QGLShader::Fragment, this);

    if( VideoSrcIsYUV )
    {
	    #ifdef ARM
        fshader1->compileSourceFile( QString("yuv2rgb_gles.frag") );
        #else
        fshader1->compileSourceFile( QString("yuv2rgb_gl.frag") );
        #endif
    }
    else
    {
        #ifdef ARM
        fshader1->compileSourceFile( QString("rgb2rgb_gles.frag") );
        #else
        fshader1->compileSourceFile( QString("rgb2rgb_gl.frag") );
        #endif
    }

    ShaderProgram_.addShader(vshader1);
    ShaderProgram_.addShader(fshader1);
    ShaderProgram_.link();

    // Actually use the created program
    ShaderProgram_.bind();

    ShaderProgram_.setUniformValue(ShaderProgram_.uniformLocation("s_baseMap"), 0);
    ShaderProgram_.setUniformValue(ShaderProgram_.uniformLocation("texture_width"), (GLfloat)fWidth);
    ShaderProgram_.setUniformValue(ShaderProgram_.uniformLocation("texel_width"), (GLfloat)(1.0/fWidth));

    delete vshader1;
    delete fshader1;

    glGenTextures ( 1, &texId );
    //glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, texId );

    // Sets the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear ( GL_COLOR_BUFFER_BIT );
}

//-----------------------------------------------------------------------------
void GLESWidget::DoPaint(void)
//-----------------------------------------------------------------------------
{
    baseMapTexId = texId;

    if( baseMapTexId <= 0 || !AcquisitionStarted )
    {
        printf("%s(%d) ERROR\n", __FUNCTION__, __LINE__ );
        return;
    }

    GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,  // Position 0
                             0.0f,  0.0f,        // TexCoord 0
                            -1.0f, -1.0f, 0.0f,  // Position 1
                             0.0f,  1.0f,        // TexCoord 1
                             1.0f, -1.0f, 0.0f,  // Position 2
                             1.0f,  1.0f,        // TexCoord 2
                             1.0f,  1.0f, 0.0f,  // Position 3
                             1.0f,  0.0f         // TexCoord 3
                           };
    GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };

    // Set the viewport
    glViewport ( 0, 0, 640, 480 );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    GLint positionLoc = ShaderProgram_.attributeLocation("a_position");
    GLint texCoordLoc = ShaderProgram_.attributeLocation("a_texCoord");

    // Load the vertex position
    ShaderProgram_.setAttributeArray(positionLoc, vVertices, 3, 5*sizeof(GLfloat));
    // Load the texture coordinate
    ShaderProgram_.setAttributeArray(texCoordLoc, &vVertices[3], 2, 5*sizeof(GLfloat));

    ShaderProgram_.enableAttributeArray(positionLoc);
    ShaderProgram_.enableAttributeArray(texCoordLoc);

    //glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, baseMapTexId );

    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices );

    glDeleteTextures ( 1, &baseMapTexId );
    
    VideoSyncMutexUnlock();
}

//-----------------------------------------------------------------------------
void GLESWidget::paintGL()
//-----------------------------------------------------------------------------
{
    if(AcquisitionStarted)
    {
        DoPaint();
        IsInitialised = true;
    }
    else if( !IsInitialised )
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear ( GL_COLOR_BUFFER_BIT );
    }
}

//-----------------------------------------------------------------------------
void GLESWidget::VideoEnable( bool enable )
//-----------------------------------------------------------------------------
{
    AcquisitionStarted = enable;
}


