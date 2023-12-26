#include "mainwindow.h"
#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("simple music player");
    // 创建播放器对象
    QMediaPlayer *player = new QMediaPlayer;
    // 创建调节声音的对象
    QAudioOutput *audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    // 创建选择文件按钮
    QPushButton *openButton = new QPushButton("选择音乐文件");
    // 将按钮的click事件和选择音乐文件的函数connect
    QObject::connect(openButton, &QPushButton::clicked, [&](){
        QString filePath = QFileDialog::getOpenFileName(nullptr, "选择音乐文件", QDir::homePath(), "音乐文件 (*.mp3 *.wav *.ogg)");
        if (!filePath.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(filePath));
            player->play();
            qDebug() << "Playback state:" << player->playbackState();
        }
    });
    // 创建停止播放按钮
    QPushButton *stopButton = new QPushButton("停止播放");
    // 将其和mediaplayer的stop connect起来
    QObject::connect(stopButton, &QPushButton::clicked, player, &QMediaPlayer::stop);
    // 创建音量调节板块
    // 创建音量调节滑块
    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    QObject::connect(volumeSlider, &QSlider::valueChanged, audioOutput, &QAudioOutput::setVolume);
    // 创建布局管理器, 管理程序内部的布局
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(openButton);
    layout->addWidget(stopButton);
    layout->addWidget(volumeSlider);
    // 将布局管理器设置给主窗口
    window.setLayout(layout);
    // 显示主窗口
    window.show();
    return app.exec();
}
