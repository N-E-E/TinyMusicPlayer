#ifndef K_GLOBAL_INFO_H__
#define K_GLOBAL_INFO_H__

#include <QMap>
#include <QPixmap>

enum class PlayState {
    SuspendType,
    PlayingType,
    PlayOverType,
    NoSelectedMusicType
};

enum class PlayMode {
    ReplayType,
    SeqenceType,
    RandomType
};


enum class MusicType {
    RemoteMusicType,
    LocalMusicType,
    RawMusicType
};

extern QMap<QString, QString> qssTable;

QPixmap pixmapToRound(QPixmap& src, int radius);

#endif // !K_GLOBAL_INFO_H__
