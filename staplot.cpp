#include "staplot.h"

StaPlot::StaPlot(QString username, QHash<QString, double> data,  QWidget* parent )
    : QMainWindow( parent )
{
    BarChart* chart = new BarChart(username, data);
    setCentralWidget( chart );

    QToolBar* toolBar = new QToolBar();

    QToolButton* btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    connect( btnExport, SIGNAL(clicked()), chart, SLOT(exportChart()));

    toolBar->addWidget( btnExport );
    addToolBar( toolBar );
    resize(1600, 900);
}

void StaPlot::closeEvent(QCloseEvent *e)
{
}
