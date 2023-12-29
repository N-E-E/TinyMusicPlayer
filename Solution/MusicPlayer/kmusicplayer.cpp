#include "kmusicplayer.h"
#include "kmusiclistitem.h"

#include <QMatrix>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include <QFileDialog>
#include <QMessageBox>

KMusicPlayer::KMusicPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    // frameless
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);  //设置窗口透明化


    // init btn style
    QFile qss(qssTable["playbtn"]);
    qss.open(QFile::ReadOnly);
    ui.suspendAndResumeButton->setStyleSheet(qss.readAll());
    qss.close();

    // init playmode
    QFile qss_playmode(qssTable["replaymode"]);
    qss_playmode.open(QFile::ReadOnly);
    ui.playMode->setStyleSheet(qss_playmode.readAll());
    qss_playmode.close();

    m_playMode = PlayMode::RandomType;

    // init
    m_musicModule = new KMusicModule(this);
    m_lyricModule = new KLyricModule(this);
    m_localDataManager = new KLocalMusicManager(this);
    m_remoteDataManager = new KRemoteDataManager(this);

    // local list
    initLocalMusicList();

    // remote list(hide at first)
    m_remoteDataManager->parseAll();
    initRemoteMusicList();
    ui.remoteMusicList->hide();

    // init volume bar
    ui.volumeSlider->setSliderPosition(m_musicModule->getVolume());

    // remote/local/raw switch
    (void)connect(ui.localMusicBtn, &QPushButton::clicked, this, &KMusicPlayer::switchToLocalMode);
    (void)connect(ui.remoteMusicBtn, &QPushButton::clicked, this, &KMusicPlayer::switchToRemoteMode);

    (void)connect(ui.openFileBtn, &QPushButton::clicked, this, &KMusicPlayer::openRawMusicFromFolder);
    (void)connect(this, &KMusicPlayer::sigPlayRawMusic, this, &KMusicPlayer::playRawMusic);

    // suspend/resume
    (void)connect(ui.suspendAndResumeButton, &QPushButton::clicked, this, &KMusicPlayer::resumeOrSuspendCurMusic);

    // auto play
    (void)connect(m_musicModule, &KMusicModule::sigMusicPlayOver, this, &KMusicPlayer::updateWhenPlayOver);
    (void)connect(this, &KMusicPlayer::sigPlayMusic, this, &KMusicPlayer::playMusic);

    // next/prev music btn -> play
    (void)connect(ui.playNextButton, &QPushButton::clicked, this, &KMusicPlayer::playNextMusic);
    (void)connect(ui.playPrevButton, &QPushButton::clicked, this, &KMusicPlayer::playPrevMusic);

    // update sliderbar and time display.
    (void)connect(m_musicModule, &KMusicModule::sigUpdateProgress, this, &KMusicPlayer::updateMusicTime);
    (void)connect(m_musicModule, &KMusicModule::sigUpdateProgress, this, &KMusicPlayer::updateSliderPosition);
    (void)connect(ui.audioSliderBar, &QSlider::sliderMoved, this, &KMusicPlayer::setProgress);

    // img rotate when music is playing.
    (void)connect(m_musicModule, &KMusicModule::sigUpdateProgress, this, &KMusicPlayer::rotateImgWhenPlaying);

    // update lyric when music is playing
    (void)connect(m_musicModule, &KMusicModule::sigUpdateProgress, this, &KMusicPlayer::updateLyric);

    // play mode
    (void)connect(ui.playMode, &QPushButton::clicked, this, &KMusicPlayer::switchPlayMode);

    // set volume
    (void)connect(ui.volumeSlider, &QSlider::sliderMoved, this, &KMusicPlayer::setVolume);
    (void)connect(this, &KMusicPlayer::sigSetVolume, this, &KMusicPlayer::setVolume);
    (void)connect(ui.volumeIcon, &QPushButton::clicked, this, &KMusicPlayer::setVolumeState);

    (void)connect(ui.closeBtn, &QPushButton::clicked, this, &KMusicPlayer::close);
}

KMusicPlayer::~KMusicPlayer()
{
    if (m_musicModule) delete m_musicModule;
    if (m_lyricModule) delete m_lyricModule;

    if (m_localDataManager) delete m_localDataManager;
    if (m_remoteDataManager) delete m_remoteDataManager;

    for (auto ele : m_localMusicList) {
        if (ele) delete ele;
    }
    for (auto ele : m_remoteMusicList) {
        if (ele) delete ele;
    }

    if (m_trayMennu) delete m_trayMennu;
    if (m_systemTray) delete m_systemTray;
    if (m_actionShow) delete m_actionShow;
    if (m_actionQuit) delete m_actionQuit;

}

void KMusicPlayer::initLocalMusicList()
{
    ui.localMusicList->setResizeMode(QListView::Adjust);
    ui.localMusicList->setViewMode(QListView::ListMode);
    for (auto i = 0; i < m_localDataManager->size(); i++) {
        LocalMusicMetaInfo info = m_localDataManager->at(i);
        localListAddItem(i, info.musicName, info.singer, info.albumName, QString::number(info.duration));
    }
}

void KMusicPlayer::initRemoteMusicList()
{
    ui.remoteMusicList->setResizeMode(QListView::Adjust);
    ui.remoteMusicList->setViewMode(QListView::ListMode);
    for (auto i = 0; i < m_remoteDataManager->size(); i++) {
        MetaInfo info = m_remoteDataManager->at(i);
        remoteListAddItem(i, info.musicName, info.singer, info.albumName, QString::number(info.duration));
    }
}


void KMusicPlayer::localListAddItem(const size_t& musicIdx, const QString& title, const QString& singer, const QString& album, const QString& duration)
{
    KMusicListItem* musicListItem = new KMusicListItem(this);
    musicListItem->setData(musicIdx, "", title, singer, album, duration);
    // connect the item with actions
    (void)connect(musicListItem, &KMusicListItem::clicked, this, &KMusicPlayer::playMusic);
    // kepp the items in a vector
    m_localMusicList.push_back(musicListItem);

    QListWidgetItem* item = new QListWidgetItem();  // TODO: whether to keep?
    item->setSizeHint(QSize(500, 75));
    ui.localMusicList->addItem(item);
    ui.localMusicList->setItemWidget(item, musicListItem);
}

void KMusicPlayer::remoteListAddItem(const size_t& musicIdx, const QString& title, const QString& singer, const QString& album, const QString& duration)
{
    KMusicListItem* musicListItem = new KMusicListItem(this);
    musicListItem->setData(musicIdx, "", title, singer, album, duration);
    // TODO: connect item with actions
    (void)connect(musicListItem, &KMusicListItem::clicked, this, &KMusicPlayer::playMusic);
    m_remoteMusicList.push_back(musicListItem);

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(500, 75));
    ui.remoteMusicList->addItem(item);
    ui.remoteMusicList->setItemWidget(item, musicListItem);
}

void KMusicPlayer::loadAlbumImg(const QString& path)
{
    m_curAlbumImg = QPixmap(path);
    // TODO: if path not exist
    m_curAlbumImg = m_curAlbumImg.scaled(QSize(m_curAlbumImg.width(), m_curAlbumImg.height()), Qt::IgnoreAspectRatio);
    m_curAlbumImg = pixmapToRound(m_curAlbumImg, 65);
    ui.albumImg->setPixmap(m_curAlbumImg);
}

void KMusicPlayer::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void KMusicPlayer::creatMenu()
{
    m_trayMennu = new QMenu(this);
    //新增菜单项---显示主界面
    m_trayMennu->addAction(m_actionShow);
    //增加分隔符
    m_trayMennu->addSeparator();
    //新增菜单项---退出程序
    m_trayMennu->addAction(m_actionQuit);
    //把QMenu赋给QSystemTrayIcon对象
    m_systemTray->setContextMenu(m_trayMennu);
}

void KMusicPlayer::creatAction()
{
    m_actionShow = new QAction("显示主界面", this);
    connect(m_actionShow, &QAction::triggered, this, &KMusicPlayer::slotActionMain);

    m_actionQuit = new QAction("退出", this);
    connect(m_actionQuit, &QAction::triggered, this, &KMusicPlayer::slotActionExitApp);
}

void KMusicPlayer::closeEvent(QCloseEvent* event)
{
    // 忽略关闭事件
    event->ignore();

    // 最小化到后台
    this->hide();

    m_systemTray = new QSystemTrayIcon();
    m_systemTray->setIcon(QIcon(":/images/icon.png"));     // 托盘时显示的图片
    m_systemTray->setToolTip("播放器");                                             // 鼠标在托盘图片时的提示
    //给QSystemTrayIcon添加槽函数
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &KMusicPlayer::on_activatedSysTrayIcon);
    //建立托盘操作的菜单
    creatAction();
    creatMenu();
    m_systemTray->show();   // 显示图片图标
}


void KMusicPlayer::mousePressEvent(QMouseEvent* event)
{
    int x = ui.topWidget->x();
    int y = ui.topWidget->y();
    int w = ui.topWidget->width();
    int h = ui.topWidget->height();
    //只能是鼠标左键移动和改变大小
    if ((event->button() == Qt::LeftButton) //处于左键状态
        && (event->x() > x && event->x() < x + w)  //x坐标位置判定
        && (event->y() > y && event->y() < y + h)) //y坐标位置判定
    {
        m_leftMousePressed = true;  //标志位置为真
        //按下时鼠标左键时，窗口在屏幕中的坐标
        m_dragStartPoint = event->globalPos();
    }
}


void KMusicPlayer::mouseMoveEvent(QMouseEvent* event)
{
    //移动窗口,由于取消了标题栏，因此需要自己实现拖动窗口功能
    if (m_leftMousePressed)
    {
        QPoint curPoint = event->globalPos();   //按住移动时的位置
        QPoint movePoint = curPoint - m_dragStartPoint; //与初始坐标做差，得位移
        //普通窗口
        QPoint mainWinPos = this->pos();
        //设置窗口的全局坐标
        this->move(mainWinPos.x() + movePoint.x(), mainWinPos.y() + movePoint.y());
        m_dragStartPoint = curPoint;
    }
}

void KMusicPlayer::mouseReleaseEvent(QMouseEvent* event)
{
    m_leftMousePressed = false;  //释放鼠标，标志位置为假
}

void KMusicPlayer::resumeOrSuspendCurMusic()
{
    if (m_playState == PlayState::PlayingType) {
        // update btn style --> change to start icon(means we are pausing now)
        QFile qss(qssTable["suspendbtn"]);
        qss.open(QFile::ReadOnly);
        ui.suspendAndResumeButton->setStyleSheet(qss.readAll());
        qss.close();
        // suspend.
        m_musicModule->suspend();
        m_playState = PlayState::SuspendType;
    }
    else if (m_playState == PlayState::SuspendType) {
        // update btn style --> change to pause icon(means we are playing now)
        QFile qss(qssTable["playbtn"]);
        qss.open(QFile::ReadOnly);
        ui.suspendAndResumeButton->setStyleSheet(qss.readAll());
        qss.close();
        // resume
        m_musicModule->resume();
        m_playState = PlayState::PlayingType;
    }
    else {
        // TODO: no selected music.
    }

}

void KMusicPlayer::playNextMusic()
{
    emit playMusic(getNextMusic());
}

void KMusicPlayer::playPrevMusic()
{
    emit playMusic(getPrevMusic());
}

void KMusicPlayer::playMusic(const size_t& musicIdx)
{
    // init btn style
    QFile qss(qssTable["playbtn"]);
    qss.open(QFile::ReadOnly);
    ui.suspendAndResumeButton->setStyleSheet(qss.readAll());
    qss.close();

    if (m_MusicType == MusicType::LocalMusicType) {
        playLocalMusic(musicIdx);
    }
    else if (m_MusicType == MusicType::RemoteMusicType){
        playRemoteMusic(musicIdx);
    }
    else {
        // do nothing.
    }
}

void KMusicPlayer::playLocalMusic(const size_t& localMusicIdx)
{
    QString musicFolderName = m_localDataManager->at(localMusicIdx).path;
    ui.musicTitle->setText(m_localDataManager->at(localMusicIdx).musicName);
    QString filename = "./local" + musicFolderName + "/music.mp3";
    m_musicModule->play(filename);
    m_musicModule->setVolume(m_volume);

    QString imgPath = "./local" + musicFolderName + "/img.jpg";
    loadAlbumImg(imgPath);

    // lyric part
    QString lyricPath = "./local" + musicFolderName + "/lyric.txt";
    m_lyricModule->open(lyricPath);

    m_playState = PlayState::PlayingType;
    m_curMusicIdxLocal = localMusicIdx;
}

void KMusicPlayer::playRemoteMusic(const size_t& remoteMusicIdx)
{
    m_musicModule->suspend();  // avoid the prev music is still playing
    m_playState = PlayState::SuspendType;

    // get remote url.
    QString musicPath = m_remoteDataManager->at(remoteMusicIdx).path;
    ui.musicTitle->setText(m_remoteDataManager->at(remoteMusicIdx).musicName);
    musicPath = musicPath.remove(0, 1);  // remove "/"
    QString remoteUrl = m_remoteDataManager->getRootUrl() + musicPath;

    // fetch all resource in one time.
    QString resourceFolder = m_remoteDataManager->fetchResource(remoteUrl, "/" + musicPath);

    m_musicModule->play(resourceFolder + "/music.mp3");
    m_musicModule->setVolume(m_volume);

    loadAlbumImg(resourceFolder + "/img.jpg");
    m_lyricModule->open(resourceFolder + "/lyric.txt");

    m_playState = PlayState::PlayingType;
    m_curMusicIdxRemote = remoteMusicIdx;
}

void KMusicPlayer::playRawMusic(const QString& filename)
{
    // init btn style
    QFile qss(qssTable["playbtn"]);
    qss.open(QFile::ReadOnly);
    ui.suspendAndResumeButton->setStyleSheet(qss.readAll());
    qss.close();

    m_MusicType = MusicType::RawMusicType;
    m_musicModule->suspend();  // avoid the prev music is still playing
    m_playState = PlayState::SuspendType;

    ui.musicTitle->setText("未知歌曲");
    m_musicModule->play(filename);
    m_musicModule->setVolume(m_volume);

    loadAlbumImg("./images/commonimg.png");
    m_playState = PlayState::PlayingType;
}

void KMusicPlayer::switchToRemoteMode()
{
    // reset
    //m_playState = PlayState::NoSelectedMusicType;
    //m_curAlbumImg = QPixmap();
    //m_rotateAngle = 0;
    
    ui.localMusicList->hide();
    ui.remoteMusicList->show();
    m_MusicType = MusicType::RemoteMusicType;
}

void KMusicPlayer::switchToLocalMode()
{
    // reset
    //m_playState = PlayState::NoSelectedMusicType;
    //m_curAlbumImg = QPixmap();
    //m_rotateAngle = 0;
    
    ui.remoteMusicList->hide();
    ui.localMusicList->show();
    m_MusicType = MusicType::LocalMusicType;
}

size_t KMusicPlayer::getNextMusic()
{
    if (m_MusicType == MusicType::LocalMusicType) {
        m_curMusicIdxLocal = (m_curMusicIdxLocal + 1) % m_localDataManager->size();
        return m_curMusicIdxLocal;
    }
    else if (m_MusicType == MusicType::RemoteMusicType) {
        m_curMusicIdxRemote = (m_curMusicIdxRemote + 1) % m_remoteDataManager->size();
        return m_curMusicIdxRemote;
    }
    else {
        // TODO
    }
}

size_t KMusicPlayer::getPrevMusic()
{
    if (m_MusicType == MusicType::LocalMusicType) {
        m_curMusicIdxLocal = (m_curMusicIdxLocal - 1 + m_localDataManager->size()) % m_localDataManager->size();
        return m_curMusicIdxLocal;
    }
    else if (m_MusicType == MusicType::RemoteMusicType) {
        m_curMusicIdxRemote = (m_curMusicIdxRemote - 1 + m_remoteDataManager->size()) % m_remoteDataManager->size();
        return m_curMusicIdxRemote;
    }
    else {
        // TODO
    }
}

size_t KMusicPlayer::getNextSeqence()
{
    return getNextMusic();
}

size_t KMusicPlayer::getNextRandom()
{
    if (m_MusicType == MusicType::LocalMusicType) {
        return qrand() % m_localDataManager->size();
    }
    else if (m_MusicType == MusicType::RemoteMusicType) {
        return qrand() % m_remoteDataManager->size();
    }
    else {
        // TODO
    }
    return 0;
}

size_t KMusicPlayer::getNextReplay()
{
    if (m_MusicType == MusicType::LocalMusicType) {
        return m_curMusicIdxLocal;
    }
    else if (m_MusicType == MusicType::RemoteMusicType) {
        return m_curMusicIdxRemote;
    }
    else {
        // TODO
    }
}

void KMusicPlayer::updateMusicTime()
{
    if (m_playState == PlayState::NoSelectedMusicType) {
        ui.curTimeLabel->setText("--:--");
        ui.totalTimeLabel->setText("--:--");
    }
    else {
        ui.curTimeLabel->setText(m_musicModule->getCurTimeStr());
        ui.totalTimeLabel->setText(m_musicModule->getDurationStr());
    }
}

void KMusicPlayer::updateSliderPosition()
{
    ui.audioSliderBar->setSliderPosition(static_cast<int>(m_musicModule->getProgress() * 100));
}

void KMusicPlayer::setProgress(int position)
{
    m_musicModule->setProgress(qreal(position / 100.0));
}

void KMusicPlayer::rotateImgWhenPlaying()
{
    QMatrix matrix;
    matrix.rotate(m_rotateAngle);
    if (m_rotateAngle++ == 720)
        m_rotateAngle = 0;

    ui.albumImg->setPixmap(m_curAlbumImg.transformed(matrix, Qt::SmoothTransformation));
    ui.albumImg->setAlignment(Qt::AlignCenter);
}

void KMusicPlayer::updateWhenPlayOver()
{
    // reset some info
    m_playState = PlayState::NoSelectedMusicType;
    m_rotateAngle = 0;

    if (m_MusicType == MusicType::RawMusicType) {
        ui.curTimeLabel->setText("--:--");
        ui.totalTimeLabel->setText("--:--");
        ui.audioSliderBar->setSliderPosition(0);
        return;
    }

    switch (m_playMode)
    {
    case PlayMode::ReplayType:
        emit sigPlayMusic(getNextReplay());
        break;
    case PlayMode::SeqenceType:
        emit sigPlayMusic(getNextSeqence());
        break;
    case PlayMode::RandomType:
        emit sigPlayMusic(getNextRandom());
        break;
    default:
        break;
    }
}

void KMusicPlayer::updateLyric()
{
    if (m_MusicType == MusicType::RawMusicType) {
        ui.lyricPrev->setText("");
        ui.lyricCur->setText("当前歌曲无匹配歌词");
        ui.lyricNext->setText("");
        return;
    }
    LyricGroup group = m_lyricModule->match(m_musicModule->getCurTime());
    ui.lyricPrev->setText(group.prev);
    ui.lyricCur->setText(group.cur);
    ui.lyricNext->setText(group.next);
}

void KMusicPlayer::setVolume(int volume)
{
    m_volume = volume;
    if (m_playState == PlayState::PlayingType) {  // change only when music is playing.
        m_musicModule->setVolume(m_volume);
    }
}

void KMusicPlayer::setVolumeState()
{
    if (m_silence) {
        m_silence = false;

        QFile qss(qssTable["volumenormal"]);
        qss.open(QFile::ReadOnly);
        ui.volumeIcon->setStyleSheet(qss.readAll());
        qss.close();

        ui.volumeSlider->setSliderPosition(30);
        emit sigSetVolume(30);
    }
    else {
        m_silence = true;

        QFile qss(qssTable["volumesilence"]);
        qss.open(QFile::ReadOnly);
        ui.volumeIcon->setStyleSheet(qss.readAll());
        qss.close();

        ui.volumeSlider->setSliderPosition(0);
        emit sigSetVolume(0);
    }
}

void KMusicPlayer::switchPlayMode()
{
    if (m_playMode == PlayMode::SeqenceType) {
        QFile qss(qssTable["randommode"]);
        qss.open(QFile::ReadOnly);
        ui.playMode->setStyleSheet(qss.readAll());
        qss.close();

        m_playMode = PlayMode::RandomType;
    }
    else if (m_playMode == PlayMode::RandomType) {
        QFile qss(qssTable["replaymode"]);
        qss.open(QFile::ReadOnly);
        ui.playMode->setStyleSheet(qss.readAll());
        qss.close();

        m_playMode = PlayMode::ReplayType;
    }
    else {  // replymode
        QFile qss(qssTable["seqmode"]);
        qss.open(QFile::ReadOnly);
        ui.playMode->setStyleSheet(qss.readAll());
        qss.close();

        m_playMode = PlayMode::SeqenceType;
    }
}

void KMusicPlayer::openRawMusicFromFolder()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("select mp3 file"),
        "./",
        tr("audio files(*.mp3 *.flac *.wav)"));
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Warning!", "Failed to open the video!");
    }
    else {
        emit sigPlayRawMusic(fileName);
    }
}

void KMusicPlayer::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        m_systemTray->hide();
        break;
    default:
        break;
    }
}

void KMusicPlayer::slotActionMain()
{
    this->show();
    m_systemTray->hide();
}

void KMusicPlayer::slotActionExitApp()
{
    exit(0);
}


