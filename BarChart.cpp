/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "BarChart.h"

#include <QwtPlotRenderer>
#include <QwtPlotMultiBarChart>
#include <QwtColumnSymbol>
#include <QwtPlotLayout>
#include <QwtLegend>
#include <QwtScaleDraw>
#include <QwtText>
#include <QwtMath>

namespace
{
    class DistroScaleDraw : public QwtScaleDraw
    {
    public:
        DistroScaleDraw(const QStringList& labels)
            : m_labels(labels)
        {

        }
        virtual QwtText label( double value ) const QWT_OVERRIDE
        {
            QwtText lbl;
            const int index = qRound( value );
            if ( index >= 0 && index < m_labels.size() )
            {
                lbl = m_labels[ index ];
            }
            return lbl;
        }
    private:
      const QStringList m_labels;

    };
}
BarChart::BarChart(QString _username, QHash<QString, double> d, QWidget* parent )
    : QwtPlot( parent )
{
    this->interest = d;
    this->username = _username;
    setAutoFillBackground( true );

    setPalette( Qt::white );
    canvas()->setPalette( QColor( "LemonChiffon" ) );
    QString t = QString("%1's interest analysis results").arg(username);
    setTitle(t);

    setAxisTitle( QwtAxis::YLeft, "Attention" );
    setAxisScale(QwtAxis::YLeft,0,100,10);
    setAxisTitle( QwtAxis::XBottom, "Interset" );

    m_barChartItem = new QwtPlotMultiBarChart( "Bar Chart " );
    m_barChartItem->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    m_barChartItem->setSpacing( 20 );
    m_barChartItem->setMargin( 3 );

    m_barChartItem->attach( this );

    insertLegend( new QwtLegend() );

    populate();
    setOrientation( 0 );

    setAutoReplot( true );
}

void BarChart::populate()
{
    static const char* colors[] = { "DarkOrchid", "SteelBlue", "Gold", "Red" };
    const int numBars = 1;
    QVector< QVector< double > > series;
    QList< QwtText > titles;
    QHashIterator<QString, double> iter(interest);
    while(iter.hasNext())
    {
        iter.next();
        QVector< double > values;
        titles << iter.key();
        bottomLabel << iter.key();
        values << iter.value();
        series << values;
    }
    setAxisScaleDraw(QwtAxis::XBottom, new DistroScaleDraw(bottomLabel));
//    m_barChartItem->setBarTitles( titles );
    m_barChartItem->setLegendIconSize( QSize( 10, 14 ) );

    // 设置样式表
    for ( int i = 0; i < numBars; i++ )
    {
        QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );
        symbol->setPalette( QPalette("SteelBlue") );
        m_barChartItem->setSymbol( i, symbol );
    }
    m_barChartItem->setSamples( series );
}

void BarChart::setMode( int mode )
{
    if ( mode == 0 )
    {
        m_barChartItem->setStyle( QwtPlotMultiBarChart::Grouped );
    }
    else
    {
        m_barChartItem->setStyle( QwtPlotMultiBarChart::Stacked );
    }
}

void BarChart::setOrientation( int orientation )
{
    int axis1, axis2;

    if ( orientation == 0 )
    {
        axis1 = QwtAxis::XBottom;
        axis2 = QwtAxis::YLeft;

        m_barChartItem->setOrientation( Qt::Vertical );
    }
    else
    {
        axis1 = QwtAxis::YLeft;
        axis2 = QwtAxis::XBottom;

        m_barChartItem->setOrientation( Qt::Horizontal );
    }

    setAxisScale( axis1, 0, m_barChartItem->dataSize() - 1, 1.0 );
    setAxisAutoScale( axis2 );

    QwtScaleDraw* scaleDraw1 = axisScaleDraw( axis1 );
    scaleDraw1->enableComponent( QwtScaleDraw::Backbone, false );
    scaleDraw1->enableComponent( QwtScaleDraw::Ticks, false );

    QwtScaleDraw* scaleDraw2 = axisScaleDraw( axis2 );
    scaleDraw2->enableComponent( QwtScaleDraw::Backbone, true );
    scaleDraw2->enableComponent( QwtScaleDraw::Ticks, true );

    plotLayout()->setAlignCanvasToScale( axis1, true );
    plotLayout()->setAlignCanvasToScale( axis2, false );

    plotLayout()->setCanvasMargin( 0 );
    updateCanvasMargins();

    replot();
}

void BarChart::exportChart()
{
    QwtPlotRenderer renderer;
    QString t = QString("%1's interest analysis results").arg(username);
    renderer.exportTo( this, t+".pdf");
}
