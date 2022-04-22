#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_layout.h>
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QVector>
#include <QDateTime>
#include "timescaledraw.h"
#include "tgamparser.h"
#include "database.h"
#include <QPushButton>
#include "videoplayer.h"

namespace  {
    class EEGCurve : public QwtPlotCurve
    {
    public:
        EEGCurve(const QString &title):QwtPlotCurve(title)
        {
            setRenderHint( QwtPlotItem::RenderAntialiased, true );
        }
        void setAttr(QwtPlot &plot, bool fill, QColor color)
        {
            attach(&plot);
            if(fill){
                setPen(QPen(Qt::NoPen));
                setBrush(color);
            } else {
                setPen(color, 1);
            }
        }
    };
}

class DynPlot : public QWidget
{
    Q_OBJECT

public:
    explicit DynPlot(QWidget *parent = nullptr);
    ~DynPlot();
    void initPlots();
    QPushButton *loadVideo;
    QPushButton *quit;
    QPushButton *analyze;
    QPushButton *faceRecognize;
    QWidget *widget;
    bool canDraw = false;
    void updateTimeOnce();

private:
    void initUi();
    QwtPlot *rawWavePlot;
    QwtPlot *AMWavePlot;

    EEGCurve* rawWave;
    EEGCurve* deltaWave;
    EEGCurve* thetaWave;
    EEGCurve* lowAlphaWave;
    EEGCurve* highAlphaWave;
    EEGCurve* lowBetaWave;
    EEGCurve* highBetaWave;
    EEGCurve* lowGammaWave;
    EEGCurve* midGammaWave;
    EEGCurve* attentionWave;
    EEGCurve* meditationWave;

    QVector<double> xRawData;
    QVector<double> xWaveData;

    QVector<double> yRawData;
    QVector<double> yDeltaData;
    QVector<double> yThetaData;
    QVector<double> yLowAlpha;
    QVector<double> yHighAlpha;
    QVector<double> yLowBeta;
    QVector<double> yHighBeta;
    QVector<double> yLowGamma;
    QVector<double> yMidGamma;
    QVector<double> yAttention;
    QVector<double> yMeditation;

    unsigned int now_;

    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;

public slots:
    void updateRawWave(int);
    void updateAxisX();
    void updateWave(LargePackage);
    void RawWavePlotShowCurve(const QVariant &, bool);
    void AMWavePlotShowCurve(const QVariant &, bool);

signals:
    void loadVideoClicked();
    void replotFinish(double, LargePackage);
};

#endif // WIDGET_H
