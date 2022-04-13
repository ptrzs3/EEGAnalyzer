#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H
#include <QCoreApplication>
#include <QMessageBox>
typedef struct{
    int delta;
    int theta;
    int lowAlpha;
    int highAlpha;
    int lowBeta;
    int highBeta;
    int lowGamma;
    int midGamma;
    int attention;//0-100
    int meditation;//0-100

    void init(){
        delta=0;
        theta=0;
        lowAlpha=0;
        highAlpha=0;
        lowBeta=0;
        highBeta=0;
        lowGamma=0;
        midGamma=0;
        attention=0;
        meditation=0;
    }
}LargePackage;

#endif // IDENTIFIERS_H
