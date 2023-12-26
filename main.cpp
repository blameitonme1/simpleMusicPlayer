#include "mainwindow.h"
#include <QApplication>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>    // 连接到媒体播放器的元数据更改信号
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QDebug>
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
    // 创建音量调节滑块
    QSlider *volumeSlider = new QSlider(Qt::Vertical);
    volumeSlider->setRange(0, 100);
    QObject::connect(volumeSlider, &QSlider::valueChanged, audioOutput, [&](int value){
        audioOutput->setVolume((qfloat16)value / 100);
    } );
    // 创建一个停止播放的按钮    // 连接到媒体播放器的元数据更改信号
    QPushButton *pauseButton = new QPushButton("播放/暂停");
    QObject::connect(pauseButton, &QPushButton::clicked, [&]() {
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
        } else {
            player->play();
        }
    });
    // 创建标签以显示歌曲信息
    QLabel *titleLabel = new QLabel("歌名: ");
    QLabel *artistLabel = new QLabel("演唱者: ");
    QLabel *durationLabel = new QLabel("时长: ");
    // 创建进度条,方便手动调节歌曲进度
    QSlider *positionSlider = new QSlider(Qt::Horizontal);// 设置进度条范围为0到100，或者根据需要设置
    positionSlider->setRange(0, 100);
    // 添加前进和后退按钮
    QPushButton *forwardButton = new QPushButton("前进4%");
    QPushButton *backwardButton = new QPushButton("后退4%");

    // 连接前进和后退按钮的clicked信号到槽函数
    QObject::connect(forwardButton, &QPushButton::clicked, [&](){
        qint64 currentPosition = player->position();
        qint64 duration = player->duration();
        qint64 newPosition = currentPosition + static_cast<qint64>(0.04 * duration);

        // 避免 newPosition 超过 duration
        if (newPosition > duration) {
            newPosition = duration;
        }

        player->setPosition(newPosition);
    });

    QObject::connect(backwardButton, &QPushButton::clicked, [&](){
        qint64 currentPosition = player->position();
        qint64 duration = player->duration();
        qint64 newPosition = currentPosition - static_cast<qint64>(0.04 * duration);

        // 避免 newPosition 小于 0
        if (newPosition < 0) {
            newPosition = 0;
        }

        player->setPosition(newPosition);
    });
    // 连接到媒体播放器的位置更改信号，更新进度条
    QObject::connect(player, &QMediaPlayer::positionChanged, [&](qint64 position) {
        qint64 duration = player->duration();

        // 避免除以零错误
        if (duration > 0) {
            int percentage = static_cast<int>((position * 100) / duration);
            positionSlider->setValue(percentage);
        }
    });
    // 连接到媒体播放器的元数据更改信号
    QObject::connect(player, &QMediaPlayer::metaDataChanged, [&]() {
        qDebug() << QMediaMetaData::metaDataKeyToString(player->metaData().Title);
        titleLabel->setText("歌名: " + QMediaMetaData::metaDataKeyToString(player->metaData().Title));
        artistLabel->setText("演唱者: " + QMediaMetaData::metaDataKeyToString(player->metaData().Author));
        durationLabel->setText("时长: " + QTime(0, 0).addMSecs(player->duration()).toString("mm:ss"));
    });
    // 创建布局管理器, 管理程序内部的布局
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(20); // 设置部件之间的间距为20像素
    layout->addWidget(openButton);
    layout->addWidget(stopButton);
    layout->addWidget(volumeSlider);
    layout->addWidget(pauseButton);
    layout->addWidget(titleLabel);
    layout->addWidget(artistLabel);
    layout->addWidget(durationLabel);
    layout->addWidget(positionSlider);
    layout->addWidget(forwardButton);
    layout->addWidget(backwardButton);
    // 设置伸缩因子，增加一些弹性
    layout->setStretch(0, 1);
    layout->setStretch(1, 1);
    layout->setStretch(2, 1);
    layout->setStretch(3, 1);
    layout->setStretch(4, 1);
    // 插入可伸缩的空间，使得标签和其他部件之间有一定的间隔
    layout->addStretch(1);
    // 将布局管理器设置给主窗口
    window.setLayout(layout);
    // 增大窗口的大小
    window.resize(1000, 600);
    // 显示主窗口
    window.show();
    return app.exec();
}
