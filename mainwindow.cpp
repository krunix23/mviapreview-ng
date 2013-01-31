#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), pDevMgr(0)
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
    
    ui->actionWhitBalance->setEnabled(false);
    ui->actionAutoExposure->setEnabled(false);
	
	ComboBoxDevices = new QComboBox();

    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->mainToolBar->addSeparator();
	ui->mainToolBar->addWidget(ComboBoxDevices);
	ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionLive);
    ui->mainToolBar->addAction(ui->actionWhitBalance);
    ui->mainToolBar->addAction(ui->actionAutoExposure);
    ui->mainToolBar->addSeparator();

    pDevMgr = new DeviceManager();
	DetectDevices();
    
    iWidth = 640;
	iHeight = 480;
    
    pGLESWidget_ = new GLESWidget(iWidth, iHeight, ui->widget);

	pWorker_ = new WorkerThread(pDevMgr, this);
    pWorker_->AttachGLWidget(pGLESWidget_);
        
    connect( pWorker_, SIGNAL(DoLoadTexture(unsigned char*)), pGLESWidget_, SLOT(DoLoadTexture(unsigned char*)) );
    connect( pWorker_, SIGNAL(DoUpdateGL(int)), pGLESWidget_, SLOT(DoUpdateGL(int)) );
    
    connect( pWorker_, SIGNAL(DoUpdateStatusBar(QString)), this, SLOT(UpdateStatusBar(QString)) );
    
    connect( this, SIGNAL(DoWhiteBalanceIA(bool)), pWorker_, SLOT(DoWhiteBalanceIA(bool)) );
    connect( this, SIGNAL(DoAutoExposureIA(bool)), pWorker_, SLOT(DoAutoExposureIA(bool)) );
    
    connect( pWorker_, SIGNAL(EnableMenuActions(bool)), this, SLOT(EnableMenuActions(bool)) );
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
void MainWindow::DetectDevices(void)
//-----------------------------------------------------------------------------
{
	unsigned int devicecount_ = 0;

	if(!pDevMgr)
		return;
		
	devicecount_ = pDevMgr->deviceCount();
	if(!devicecount_)
		return;

	printf("DeviceManager is detecting devices ...\n");

	for( unsigned int i = 0; i < devicecount_; i++ )
	{
		printf("[%d]: %s\n", i, pDevMgr->getDevice(i)->serial.readS().c_str() );
		DeviceSet.insert(i);
		QString serial_( pDevMgr->getDevice(i)->serial.readS().c_str() );
		ComboBoxDevices->insertItem( i, serial_ );
	}
	
	ComboBoxDevices->setCurrentIndex(0);
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
	{
		ComboBoxDevices->setEnabled(false);
		pWorker_->OpenDevice( ComboBoxDevices->currentIndex() , iWidth, iHeight );
        pWorker_->StartThread();
	}
    else
	{
		ComboBoxDevices->setEnabled(true);
        pWorker_->StopThread();
	}
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

//-----------------------------------------------------------------------------
void MainWindow::EnableMenuActions( bool arg )
//-----------------------------------------------------------------------------
{
	ui->actionWhitBalance->setEnabled(arg);
    ui->actionAutoExposure->setEnabled(arg);
}
