#include "tgamparser.h"

TGAMParser::TGAMParser()
{
    port = new QSerialPort();
    largePackage.init();
    state = SYNC;
    validSmallPackageCounter = 0;
    validLargePackageCounter = 0;
    packageCounter = 0;
    invalidPackageCounter = 0;
    getEEGDevicePortNumber();
}

void TGAMParser::getEEGDevicePortNumber()
{
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        // 识别特定适配器
        if(info.serialNumber() == "203C366E594D" && info.productIdentifier() == 22336) {
            EEGDevicePortNumber = info.portName();
            break;
        }
    }
}
bool TGAMParser::connectEEGDevice()
{
    if(EEGDevicePortNumber.isEmpty()){
        connected = false;
        return connected;
    }
    if(port->isOpen()){
        port->clear();
        port->close();
    }
    port->setPortName(EEGDevicePortNumber);
    if(!port->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL,
                             "ERROR",
                             QString("Open TGAM adapter failed: %1")
                             .arg(EEGDevicePortNumber));
        connected = false;
        return connected;
    }
    //打开成功
    port->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    port->setDataBits(QSerialPort::Data8);		//数据位为8位
    port->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    port->setParity(QSerialPort::NoParity);	//无校验位
    port->setStopBits(QSerialPort::OneStop); //一位停止位
    connect(port, &QSerialPort::readyRead, this, &TGAMParser::readOneByteFromSerialPort);
    connected = true;
    return connected;
}
void TGAMParser::removeThree()
{
    for(int i=0; i<3; i++){
        payloadList.removeFirst();
    }
}
void TGAMParser::parsePayload()
{
    unsigned char payloadData = payloadList.first();
    payloadList.removeFirst();
    if(payloadData == 0x80){
        packageType = SMALL_PACKAGE;
        validSmallPackageCounter++;
        payloadList.removeFirst();
        rawData = payloadList[0]<<8 | payloadList[1];
        if(rawData >= 32768)
            rawData -= 65536;
        emit smallPackageReady(rawData);
        state = SYNC;
    } else if(payloadData == 0x02) {
        packageType = LARGE_PACKAGE;
        validLargePackageCounter ++;
        // 抛弃三个无用值
        removeThree();
        int pointer = 0;
        largePackage.init();
        largePackage.delta =  payloadList[pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.theta = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.lowAlpha = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.highAlpha = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.lowBeta = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.highBeta = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.lowGamma = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        largePackage.midGamma = payloadList[++pointer]<<16 | payloadList[++pointer]<<8 | payloadList[++pointer];
        if(payloadList[++pointer] == 0x04) {
            largePackage.attention = payloadList[++pointer];
        }
        if(payloadList[++pointer] == 0x05) {
            largePackage.meditation = payloadList[++pointer];
        }
        emit largePackageReady(largePackage);
    }
    state = SYNC;
}

void TGAMParser::parseByte(char ch)
{
    unsigned char byte = ch;
    switch (state) {
    case SYNC:
        payloadLength = 0;
        payloadCounter = 0;
        payloadSum = 0;
        payloadList.clear();
        if(byte == 0xAA){
            state = SYNC_CHECK;
        } else {
            state = SYNC;
        }
        break;
    case SYNC_CHECK:
        if(byte == 0xAA) {
            state = PAYLOAD_LENGTH;
        } else {
            state = SYNC;
        }
        break;
    case PAYLOAD_LENGTH:
        if(byte >= 170){
            state = SYNC;
        }else{
            payloadLength = byte;
            state = RECEIVE_PAYLOAD;
            payloadList.clear();
        }
        break;
    case RECEIVE_PAYLOAD:
        if(payloadCounter < payloadLength){
            payloadSum += byte;
            payloadList.append(byte);
            payloadCounter ++;
            if(payloadCounter == payloadLength){
                state = CHECK_SUM;
            } else {
                state = RECEIVE_PAYLOAD;
            }
        } else{
            state = SYNC;
        }
        break;
    case CHECK_SUM:
        payloadSum &= 0xff;
        payloadSum = ~payloadSum & 0xff;
        if(payloadSum == byte){
            parsePayload();
            packageCounter++;
        } else {
            invalidPackageCounter ++;
            if(invalidPackageCounter/packageCounter > 0.1 && !showOnce
                    && invalidPackageCounter > 30){
                showOnce = true;
                QMessageBox::warning(NULL,
                                     "TGAM module",
                                     "The packet loss rate is too high, "
                                     "please consider testing in a place without electromagnetic interference");
            }
            state = SYNC;
        }
        break;
    }
}


void TGAMParser::readOneByteFromSerialPort()
{
    char ch;
    while(port->read(&ch, 1)){
            parseByte(ch);
        }
}

int TGAMParser::getAttention()
{
    return largePackage.attention;
}

int TGAMParser::getMeditation()
{
    return largePackage.meditation;
}

int TGAMParser::getDelta()
{
    return largePackage.delta;
}
int TGAMParser::getTheta()
{
    return largePackage.theta;
}
int TGAMParser::getLowAlpha()
{
    return largePackage.lowAlpha;
}
int TGAMParser::getHighAlpha()
{
    return largePackage.highAlpha;
}
int TGAMParser::getHighBeta()
{
    return largePackage.highBeta;
}
int TGAMParser::getLowBeta()
{
    return largePackage.lowBeta;
}
int TGAMParser::getLowGamma()
{
    return largePackage.lowGamma;
}
int TGAMParser::getMidGamma()
{
    return largePackage.midGamma;
}
