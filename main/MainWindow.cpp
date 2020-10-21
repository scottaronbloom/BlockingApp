#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SABUtils/ThreadedProgressDialog.h"

#include <QDebug>
#include <QAbstractNativeEventFilter>
#include <QThread>

class MyNativeEventFilter: public QAbstractNativeEventFilter
{
public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void* message, long* param)
    {
        (void)eventType;
        (void)message;
        (void)param;
        qDebug() << "Inside nativeEventFilter";
        return false;
    }
};

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    //QApplication::instance()->installNativeEventFilter(new MyNativeEventFilter);
    connect(ui->PB_OK, SIGNAL(clicked()), this, SLOT(slotThatTakesTime()));

    QThread::currentThread()->setObjectName( "Main GUI Thread" );
}

#define kILOOPCNT 2
#define kOLOOPCNT 1
void CMainWindow::processNetlistCreate()
{
    qDebug() << "processNetlistCreate: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();

    // For loop mimicing long process time
    for ( int i = 0; i < kILOOPCNT; i++ )
    {
        qDebug() << "processNetlistCreate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processNetlistCreate(): i=" << i;
        QThread::sleep( 1 );
    }
}

void CMainWindow::processStatusUpdate()
{
    // For loop mimicing long process time
    for(int i =0 ; i < kILOOPCNT; i++) {
        qDebug() << "processStatusUpdate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processStatusUpdate(): i=" << i;
        QThread::sleep(1);
    }
}

void CMainWindow::slotThatTakesTime()
{
    ui->PB_OK->setEnabled( false );
    std::function< void() > lFunc =
    [ this ]()
    {
        qDebug() << "Inside MainWindow::slotOKClicked";
        int loopCnt = 0;
        while ( loopCnt < kOLOOPCNT )
        {
            // Calling func that takes time so GUI will hang intermittently until process events is called
            qDebug() << "slotThatTakesTime inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
            qDebug() << "MainWindow::processStatusUpdate(): loopCnt=" << loopCnt;
            processNetlistCreate();
            processStatusUpdate();
            QApplication::instance()->processEvents( QEventLoop::ExcludeUserInputEvents );
            loopCnt++;
        }
    };

    NSABUtils::CThreadedProgressDialog dlg( lFunc, this );
    dlg.setLabelText( "Long Running Thread Label" );
    dlg.exec();
    ui->PB_OK->setEnabled( true );
}

CMainWindow::~CMainWindow()
{
}

