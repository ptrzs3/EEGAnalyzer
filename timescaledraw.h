#ifndef TIMESCALEDRAW_H
#define TIMESCALEDRAW_H

#include <qwt_scale_draw.h>
#include <QDateTime>
#include <qwt_text.h>
class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw();

    virtual QwtText label(double v) const;
};

#endif // TIMESCALEDRAW_H
