#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent):
    QWidget (parent)
{
    player = new QMediaPlayer();
    vLayout = new QVBoxLayout(this);
    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    vLayout->addWidget(videoWidget);
    videoWidget->show();
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags( Qt::FramelessWindowHint);
    QList<QScreen*> screenCounter = QGuiApplication::screens();
    if(screenCounter.size() == 2){
        this->setGeometry(screenCounter[0]->geometry().width(),0,
                screenCounter[1]->geometry().width(),screenCounter[1]->geometry().height());
        secondaryScreen = true;
    } else {
        this->setGeometry(400,400, 1600, 900);
    }
}

bool VideoPlayer::play(QString videoFilePath)
{
    player->setMedia(QUrl::fromLocalFile(videoFilePath));
    player->play();
//    player->setPlaybackRate(2);
//    player->setVolume(0);
    this->show();
    return true;
//    return false;
}
