#ifndef __K_MUSIC_PLAYER_H__
#define __K_MUSIC_PLAYER_H__


#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include "ui_kmusicplayer.h"
#include "kmusicmodule.h"
#include "klocalmusicmanager.h"
#include "klyricmodule.h"
#include "kmusiclistitem.h"
#include "kremotedatamanager.h"
//#include "kglobalinfo.h"

class KMusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    KMusicPlayer(QWidget *parent = nullptr);
    ~KMusicPlayer();

    void initLocalMusicList();
    void initRemoteMusicList();

    void localListAddItem(const size_t& musicIdx, const QString& title, const QString& singer, const QString& album, const QString& duration);
    void remoteListAddItem(const size_t& musicIdx, const QString& title, const QString& singer, const QString& album, const QString& duration);

    void loadAlbumImg(const QString& path);

    virtual void paintEvent(QPaintEvent* event) override; //重绘

    // tray icon
    void creatMenu();
    void creatAction();
    void closeEvent(QCloseEvent* event) override;

    // override move action
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void sigRotateImg();
    // use in auto play.
    void sigPlayMusic(const size_t& nextMusicIdx);
    void sigSetVolume(int volume);

    // play raw music.
    void sigPlayRawMusic(QString filename);

public slots:
    void resumeOrSuspendCurMusic();
    void playNextMusic();
    void playPrevMusic();

    void playMusic(const size_t& musicIdx);
    void playLocalMusic(const size_t& localMusicIdx);
    void playRemoteMusic(const size_t& remoteMusicIdx);
    void playRawMusic(const QString& filename);

    // switch local/remote
    void switchToRemoteMode();
    void switchToLocalMode();

    size_t getNextMusic();
    size_t getPrevMusic();
    size_t getNextSeqence();
    size_t getNextRandom();
    size_t getNextReplay();

    // update music time and slider bar display.
    void updateMusicTime();
    void updateSliderPosition();
    void setProgress(int position);

    // rotate the img.
    void rotateImgWhenPlaying();

    // update when a music play over
    void updateWhenPlayOver();

    // update lyric display
    void updateLyric();

    // set volume
    void setVolume(int volume);
    void setVolumeState();

    // play mode
    void switchPlayMode();

    // play raw music
    void openRawMusicFromFolder();

    // tray icon
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void slotActionMain();
    void slotActionExitApp();
private:
    Ui::KMusicPlayerClass ui;

    PlayState m_playState = PlayState::NoSelectedMusicType;
    KMusicModule* m_musicModule = nullptr;

    // data manager
    KLocalMusicManager* m_localDataManager = nullptr;
    KRemoteDataManager* m_remoteDataManager = nullptr;

    QPixmap m_curAlbumImg;
    qint8 m_rotateAngle = 0;

    bool m_silence = false;

    KLyricModule* m_lyricModule = nullptr;

    PlayMode m_playMode = PlayMode::ReplayType;

    QVector<KMusicListItem*> m_localMusicList;
    QVector<KMusicListItem*> m_remoteMusicList;  // remote list.

    size_t m_curMusicIdxLocal = 0;
    size_t m_curMusicIdxRemote = 0;

    MusicType m_MusicType = MusicType::LocalMusicType;  // if use remote music list;

    bool m_leftMousePressed = true;
    QPoint m_dragStartPoint;

    int m_volume = 30;

    /*系统托盘*/
    QMenu* m_trayMennu;            //系统托盘右键菜单项
    QSystemTrayIcon* m_systemTray; //系统托盘图标

    //右键菜单栏选项
    QAction* m_actionShow;
    QAction* m_actionQuit;
};

#endif // !__K_MUSIC_PLAYER_H__

