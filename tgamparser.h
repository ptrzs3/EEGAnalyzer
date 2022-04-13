#ifndef TGAMPARSER_H
#define TGAMPARSER_H
#include <QSerialPort>
#include <QDebug>
#include <QSerialPortInfo>
#include <QStringList>
#include <stdio.h>
#include "identifiers.h"

class TGAMParser : public QObject
{
    Q_OBJECT
public:
    TGAMParser();
    void getEEGDevicePortNumber();
    int validSmallPackageCounter;
    int validLargePackageCounter;
    int getAttention();
    int getMeditation();
    int getDelta();
    int getTheta();
    int getLowAlpha();
    int getHighAlpha();
    int getLowBeta();
    int getHighBeta();
    int getLowGamma();
    int getMidGamma();
    QSerialPort *port;
    bool connectEEGDevice();
    bool connected = false;


private:
    void parseByte(char);
    void parsePayload();
    void removeThree();
    QString EEGDevicePortNumber;
    enum PackageType {
        SMALL_PACKAGE,
        LARGE_PACKAGE
    };
    bool showOnce = false;

    enum State{
        SYNC,
        SYNC_CHECK,
        PAYLOAD_LENGTH,
        CHECK_SUM,
        RECEIVE_PAYLOAD
    };
    PackageType packageType;
    State state;
    int payloadLength;
    int payloadCounter;
    int payloadSum;
    int rawData;
    QList<unsigned char> payloadList;
    LargePackage largePackage;
    double packageCounter;
    double invalidPackageCounter;

private slots:
    void readOneByteFromSerialPort();


signals:
    void smallPackageReady(int);
    void largePackageReady(LargePackage);
};

#endif // TGAMPARSER_H
