#ifndef STAPLOT_H
#define STAPLOT_H

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include "BarChart.h"
#include <QDebug>
#include <QApplication>
#include <QList>
#include <QDesktopWidget>
#include <QScreen>
class StaPlot : public QMainWindow
{
    Q_OBJECT
public:
    explicit StaPlot(QString, QHash<QString, double>, QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *e);
    bool secondaryScreen = false;
signals:

public slots:
};

#endif // STAPLOT_H
