/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <QwtPlot>
#include <QDebug>
class QwtPlotMultiBarChart;

class BarChart : public QwtPlot
{
    Q_OBJECT

  public:
    BarChart(QString _username, QHash<QString, double> interest, QWidget* = NULL );

  public Q_SLOTS:
    void setMode( int );
    void setOrientation( int );
    void exportChart();

  private:
    void populate();
    QHash<QString, double> interest;
    QStringList bottomLabel;
    QString username;


    QwtPlotMultiBarChart* m_barChartItem;
};
