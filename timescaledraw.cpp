#include "timescaledraw.h"
#include <QDebug>
TimeScaleDraw::TimeScaleDraw()
{

}

QwtText TimeScaleDraw::label(double v) const
{
    QDateTime dt;
    dt.setTime_t((uint)v);
    return dt.toString("mm:ss");
}
