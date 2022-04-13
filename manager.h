#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QMainWindow>
#include <windows.h>
#include <QInputDialog>
#include <QRegExp>
#include "staplot.h"
#include "database.h"
#include "dynplot.h"
#include "videoplayer.h"
#include "tgamparser.h"
class Manager : public QObject
{
public:

    Manager();
    DynPlot *dynplot;

private:
    Database *eegDatabase = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    TGAMParser *tgam = nullptr;
    bool playing = false;
    QDir videoSrc;
    QString type;
    QStringList fileFilter;
    void useSuggestion();
    QFileInfoList removeEmptyFolder(QFileInfoList);
    QString username;
    bool getTotalNumberOfRecords();
    QHash<QString, double> analyzeResult;
    void analyzeResultInitialization();
    QStringList videoTypeList;
    void getVideoTypes();
    wchar_t* QString2wchat_t(QString);

private slots:
    void playVideo();
    void videoFinish();
    void saveEEGData(double, LargePackage);
    QString executeScript(TCHAR[]);
    bool faceRecognize();
    bool faceRegistration();
    QString usernameCheck(QString);
    void analyzeInterset();
};

#endif // MANAGER_H
