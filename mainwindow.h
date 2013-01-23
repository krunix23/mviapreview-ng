#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <gleswidget.h>
#include <workerthread.h>

#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

using namespace mv;

namespace Ui {
    class MainWindow;
}

//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
//-----------------------------------------------------------------------------
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

	DeviceManager* pDevMgr;
    WorkerThread* pWorker_;
    GLESWidget* pGLESWidget_;

public:
    QWidget* GLWidget(void);
    
public slots:
    void DoUpdateGL( int reqNr );
    void UpdateStatusBar( QString message );
    void EnableMenuActions( void );

private slots:
    void on_actionLive_toggled(bool arg1);
    void on_actionWhitBalance_triggered(bool checked);
    void on_actionAutoExposure_triggered(bool checked);

signals:
    void DoWhiteBalance(void);
    void DoWhiteBalanceIA(bool arg);
    void DoAutoExposureIA(bool arg);
    void UpdateGL( int reqNr ); //unused
};

#endif // MAINWINDOW_H
