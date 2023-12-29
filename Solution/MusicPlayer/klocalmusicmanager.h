#ifndef __K_LOCAL_MUSIC_MANAGER_H__
#define __K_LOCAL_MUSIC_MANAGER_H__

#pragma execution_character_set("utf-8")


#include <QObject>
#include <QVector>
#include <QJsonObject>

class LocalMusicMetaInfo {
public:
	QString path;
	QString musicName = QString(u8"未知");
	QString singer = QString(u8"未知");
	QString albumName = QString(u8"未知");
	int duration;
};

extern const QString localMusicFolder;

class KLocalMusicManager  : public QObject
{
	Q_OBJECT

public:
	KLocalMusicManager(QObject *parent);
	~KLocalMusicManager();

	void addMusic(const QString& filename);

	void addCache(const QString& folderName);

	void loadData();

	void parseLocalMusicList(QString filename);

	LocalMusicMetaInfo parseOneMusicInfo(QJsonObject& object);

	inline LocalMusicMetaInfo at(size_t idx) {
		// TODO: check err
		return m_musicData[idx];
	}

	inline size_t size() { return m_musicData.size(); }

private:
	QVector<LocalMusicMetaInfo> m_musicData;
	QString m_localFolderRoot = "./local";
};


#endif // !__K_LOCAL_MUSIC_MANAGER_H__
