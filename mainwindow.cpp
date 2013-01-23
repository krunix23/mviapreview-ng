#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
//-----------------------------------------------------------------------------
{
    ui->setupUi(this);
    setFocus();
    setWindowIcon(QIcon(":/mv.ico"));
    setWindowTitle(QString("mviaPreview"));
    setFixedSize(664,580);

    ui->actionLive->setIcon(QIcon(":/playback_play.ico"));
    ui->actionLive->setIconText(QString("Live"));
    ui->actionWhitBalance->setIcon(QIcon(":/picture.ico"));
    ui->actionWhitBalance->setIconText(QString("AWB"));
    ui->actionAutoExposure->setIcon(QIcon(":/sun.ico"));
    ui->actionAutoExposure->setIconText(QString("AE"));

    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionLive);
    ui->mainToolBar->addAction(ui->actionWhitBalance);
    ui->mainToolBar->addAction(ui->actionAutoExposure);
    ui->mainToolBar->addSeparator();

    pDevMgr = new DeviceManager();
    
    unsigned int w = 640, h = 480;
    
    pGLESWidget_ = new GLESWidget(w, h, ui->widget);

	pWorker_ = new WorkerThread(pDevMgr, this);
    pWorker_->AttachGLWidget(pGLESWidget_);
    pWorker_->OpenDevice(w, h);
    
    connect( pWorker_, SIGNAL(DoLoadTexture(unsigned char*)), pGLESWidget_, SLOT(DoLoadTexture(unsigned char*)) );
    connect( pWorker_, SIGNAL(DoUpdateGL(int)), pGLESWidget_, SLOT(DoUpdateGL(int)) );
    
    connect( pWorker_, SIGNAL(DoUpdateStatusBar(QString)), this, SLOT(UpdateStatusBar(QString)) );
    
    connect( this, SIGNAL(DoWhiteBalanceIA(bool)), pWorker_, SLOT(DoWhiteBalanceIA(bool)) );
    connect( this, SIGNAL(DoAutoExposureIA(bool)), pWorker_, SLOT(DoAutoExposureIA(bool)) );
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
//-----------------------------------------------------------------------------
{
	delete pDevMgr;
    delete pGLESWidget_;
    delete pWorker_;
    delete ui;
}

//-----------------------------------------------------------------------------
QWidget* MainWindow::GLWidget(void)
//-----------------------------------------------------------------------------
{
    return ui->widget;
}

//-----------------------------------------------------------------------------
void MainWindow::on_actionLive_toggled(bool arg1)
//-----------------------------------------------------------------------------
{
    if( arg1 )
        pWorker_->StartThread();
    else
        pWorker_->StopThread();
}

//-----------------------------------------------------------------------------
void MainWindow::on_actionWhitBalance_triggered(bool checked)
//-----------------------------------------------------------------------------
{
    emit DoWhiteBalanceIA(checked); // do AWB via mvIA
}

//-----------------------------------------------------------------------------
void MainWindow::on_actionAutoExposure_triggered(bool checked)
//-----------------------------------------------------------------------------
{
    emit DoAutoExposureIA(checked); // do AE via mvIA
}

//-----------------------------------------------------------------------------
void MainWindow::DoUpdateGL( int reqNr )
//-----------------------------------------------------------------------------
{
    emit UpdateGL( reqNr );
}

//-----------------------------------------------------------------------------
void MainWindow::UpdateStatusBar( QString message )
//-----------------------------------------------------------------------------
{
    ui->statusBar->showMessage(message);
}
