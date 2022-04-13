#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtMultimedia>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = nullptr);
    bool play(QString);
    void pause();
    bool secondaryScreen = false;
    QMediaPlayer *player;


private:
    QVideoWidget *videoWidget;
    QVBoxLayout *vLayout;

signals:
    void Finished();

};

#endif // VIDEOPLAYER_H
