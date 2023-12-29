#ifndef __K_REMOTE_DATA_MANAGER_H__
#define __K_REMOTE_DATA_MANAGER_H__


#include "khttpdownloader.h"

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QDir>


class MetaInfo {
public:
	QString musicName = QString(u8"未知");
	QString singer = QString(u8"未知");
	QString albumName = QString(u8"未知");
	int duration;
	QString path;
	QString musicFileName;
	QString lyricFileName;
	QString imgFileName;
};


class KRemoteDataManager  : public QObject
{
	Q_OBJECT

public:
	KRemoteDataManager(QObject *parent);
	~KRemoteDataManager();

	void parseAll();
	void parseMusicMetaInfoList(QByteArray& byteArray);
	MetaInfo parseOneMusicInfo(QJsonObject& object);

	QString fetchResource(QString url, QString cachePath = "");

	inline size_t size() {
		return m_remoteMusicInfo.size();
	}

	inline MetaInfo at(size_t idx) {
		return m_remoteMusicInfo[idx];
	}

	inline QString getRootUrl() {
		return m_rootUrl;
	}


private:
	QString m_rootUrl = "https://gitee.com/MarkYangUp/music/raw/master/";

	QVector<QString> m_rootMusic = { u8"孤勇者", u8"风雨无阻" };

	QVector<MetaInfo> m_remoteMusicInfo;

	KHttpDownloader* downloader = nullptr;

	QDir* m_dir = nullptr;
};

#endif // !__K_REMOTE_DATA_MANAGER_H__
