#include "dynplot.h"

DynPlot::DynPlot(QWidget *parent) :
    QWidget(parent)
{
    initUi();
    initPlots();
}
DynPlot::~DynPlot()
{

}

void DynPlot::initUi()
{
    widget = new QWidget();
    rawWavePlot = new QwtPlot();
    AMWavePlot = new QwtPlot();
    vLayout = new QVBoxLayout(widget);
    hLayout = new QHBoxLayout();
    loadVideo = new QPushButton("Start");
    analyze = new QPushButton("Analyze");
    faceRecognize = new QPushButton("Face Recognize");
    videoCrawler = new QPushButton("Collect videos");
    quit = new QPushButton("Exit");
    hLayout->addWidget(loadVideo);
    hLayout->addWidget(analyze);
    hLayout->addWidget(faceRecognize);
    hLayout->addWidget(videoCrawler);
    hLayout->addWidget(quit);
    hLayout->setSpacing(100);
    hLayout->setContentsMargins(150,0,150,0);
    vLayout->addWidget(rawWavePlot);
    vLayout->addWidget(AMWavePlot);
    vLayout->addLayout(hLayout);
    vLayout->setSpacing(40);
    vLayout->setStretchFactor(rawWavePlot, 2);
    vLayout->setStretchFactor(AMWavePlot, 1);
    widget->setWindowIcon(QIcon(":/icon/icons/brain.ico"));
}
void DynPlot::initPlots()
{
    now_ = QDateTime::currentDateTime().toTime_t();

    rawWavePlot->setCanvasBackground( Qt::white );
    rawWavePlot->setAxisScale( QwtAxis::YLeft, 0.0, 100, 10);
    rawWavePlot->setAxisScale(QwtAxis::YRight, -2048, 2047, 496);
    rawWavePlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
    rawWavePlot->setAxisMaxMajor(QwtPlot::xBottom, 10);
    QwtLegend *legendRaw = new QwtLegend();
    legendRaw->setDefaultItemMode(QwtLegendData::Checkable);
    rawWavePlot->insertLegend(legendRaw);
    connect(legendRaw, &QwtLegend::checked, this, &DynPlot::RawWavePlotShowCurve);
    rawWavePlot->setAxisScale(QwtPlot::xBottom, now_, now_+60, 5.0);
    rawWavePlot->setAxisAutoScale(QwtAxis::YLeft, true);

    AMWavePlot->setCanvasBackground(Qt::white);
    AMWavePlot->setAxisScale( QwtAxis::YLeft, 0.0, 100, 20);
    AMWavePlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
    AMWavePlot->setAxisMaxMajor( QwtAxis::XBottom, 10);
    QwtLegend *legendAM = new QwtLegend();
    legendAM->setDefaultItemMode(QwtLegendData::Checkable);
    AMWavePlot->insertLegend(legendAM);
    connect(legendAM, &QwtLegend::checked, this, &DynPlot::AMWavePlotShowCurve);
    AMWavePlot->setAxisScale(QwtPlot::xBottom, now_, now_+60, 5.0);

    rawWave = new EEGCurve("Raw Wave");
    deltaWave = new EEGCurve("Delta");
    thetaWave = new EEGCurve("Theta");
    lowAlphaWave = new EEGCurve("Low Alpha");
    highAlphaWave = new EEGCurve("High Alpha");
    lowBetaWave = new EEGCurve("Low Beta");
    highBetaWave = new EEGCurve("High Beta");
    lowGammaWave = new EEGCurve("Low Gamma");
    midGammaWave  = new EEGCurve("Mid Gamma");
    attentionWave = new EEGCurve("Attention");
    meditationWave = new EEGCurve("Meditation");

    rawWave->setAxes(QwtAxis::XBottom, QwtAxis::YRight);
    rawWave->setAttr(*rawWavePlot, false, QColor(222,118,34));
    deltaWave->setAttr(*rawWavePlot, true, QColor(195, 195, 188, 128));
    thetaWave->setAttr(*rawWavePlot, true, QColor(178, 189, 205, 128));
    lowAlphaWave->setAttr(*rawWavePlot, true, QColor(229, 229, 154, 128));
    highAlphaWave->setAttr(*rawWavePlot, true, QColor(245, 245, 138, 128));
    lowBetaWave->setAttr(*rawWavePlot, true, QColor(171, 225, 157, 128));
    highBetaWave->setAttr(*rawWavePlot, true, QColor(145, 245, 138, 128));
    lowGammaWave->setAttr(*rawWavePlot, true, QColor(219, 170, 177, 128));
    midGammaWave->setAttr(*rawWavePlot, true, QColor(245, 145, 138, 128));
    attentionWave->setAttr(*AMWavePlot, true, QColor(145, 195, 138, 128));
    meditationWave->setAttr(*AMWavePlot, true, QColor(145, 145, 238, 128));
}

void DynPlot::updateRawWave(int rawData)
{
    if(!canDraw)
        return;
    QDateTime t = QDateTime::currentDateTime();
    double d = t.toSecsSinceEpoch();
    d += t.toMSecsSinceEpoch() % 1000 / 1000.0;
    xRawData.push_back(d);
    yRawData.push_back(rawData);
    if(xRawData.size()>33000)
    {
        xRawData.pop_front();
        yRawData.pop_front();
    }
    rawWave->setSamples(xRawData, yRawData);

    if(xRawData.last() > now_ + 60)
    {
        updateAxisX();
    }
    rawWavePlot->replot();
}
void DynPlot::updateTimeOnce()
{
    now_ = QDateTime::currentDateTime().toTime_t() - 2;
    updateAxisX();
}
void DynPlot::updateAxisX()
{
    now_+= 2;
    rawWavePlot->setAxisScale(QwtPlot::xBottom, now_, now_+60, 5.0);
    AMWavePlot->setAxisScale(QwtPlot::xBottom, now_, now_+60, 5.0);
}

void DynPlot::updateWave(LargePackage largePackage)
{
    if(!canDraw)
        return;
    double x = QDateTime::currentDateTime().toTime_t();
    xWaveData.push_back(x);

    yDeltaData.push_back(largePackage.delta);
    yThetaData.push_back(largePackage.theta);
    yLowAlpha.push_back(largePackage.lowAlpha);
    yHighAlpha.push_back(largePackage.highAlpha);
    yLowBeta.push_back(largePackage.lowBeta);
    yHighBeta.push_back(largePackage.highBeta);
    yLowGamma.push_back(largePackage.lowGamma);
    yMidGamma.push_back(largePackage.midGamma);
    yAttention.push_back(largePackage.attention);
    yMeditation.push_back(largePackage.meditation);

    if(xWaveData.size()>70) {
        xWaveData.pop_front();
        yDeltaData.pop_front();
        yThetaData.pop_front();
        yLowAlpha.pop_front();
        yHighAlpha.pop_front();
        yHighBeta.pop_front();
        yLowBeta.pop_front();
        yLowGamma.pop_front();
        yMidGamma.pop_front();
        yAttention.pop_front();
        yMeditation.pop_front();
    }

    deltaWave->setSamples(xWaveData, yDeltaData);
    thetaWave->setSamples(xWaveData, yThetaData);
    lowAlphaWave->setSamples(xWaveData, yLowAlpha);
    highAlphaWave->setSamples(xWaveData, yHighAlpha);
    lowBetaWave->setSamples(xWaveData, yLowBeta);
    highBetaWave->setSamples(xWaveData, yHighBeta);
    lowGammaWave->setSamples(xWaveData, yLowGamma);
    midGammaWave->setSamples(xWaveData, yMidGamma);
    attentionWave->setSamples(xWaveData, yAttention);
    meditationWave->setSamples(xWaveData, yMeditation);

    rawWavePlot->replot();
    AMWavePlot->replot();
    emit replotFinish(x, largePackage);
}
void DynPlot::RawWavePlotShowCurve(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = rawWavePlot->infoToItem( itemInfo );
    if ( plotItem )
        plotItem->setVisible( !on );
}
void DynPlot::AMWavePlotShowCurve(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = AMWavePlot->infoToItem( itemInfo );
    if ( plotItem )
        plotItem->setVisible( !on );
}
