#include "klocalmusicmanager.h"

#include <QFile>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextCodec>

KLocalMusicManager::KLocalMusicManager(QObject *parent)
	: QObject(parent)
{
	/*QString folderName, song, singer;
	qint8 hasLyric, hasImg, hasCache;*/
	
	// for test
	loadData();
}

KLocalMusicManager::~KLocalMusicManager()
{}

void KLocalMusicManager::addMusic(const QString & filename)
{
	//TODO
}

void KLocalMusicManager::addCache(const QString& folderName)
{
	// TODO
}

void KLocalMusicManager::loadData()
{
	QString jsonPath = m_localFolderRoot + "/list.json";
	parseLocalMusicList(jsonPath);
}

void KLocalMusicManager::parseLocalMusicList(QString filename)
{
	QFile jsonFile(filename);
	if (!jsonFile.open(QIODevice::ReadOnly)) {
		return;
	}
	QByteArray byteArray = jsonFile.readAll();
	jsonFile.close();

	QTextCodec* codec = QTextCodec::codecForName("utf-8");//指定QString的编码方式
	QString name = codec->toUnicode(byteArray);//nameArray可以是char*，可以是QByteArray
	QByteArray arr = name.toUtf8();
	//qDebug() << arr.data();

	QJsonParseError jsonError;
	QJsonDocument document = QJsonDocument::fromJson(arr, &jsonError);

	if (document.isNull() || (jsonError.error != QJsonParseError::NoError)) {
		return;
	}

	if (document.isObject()) {
		QJsonObject object = document.object();
		// parse array
		if (object.contains("list")) {
			QJsonValue value = object.value("list");
			//qDebug() << value.toArray();
			if (value.isArray()) {
				QJsonArray array = value.toArray();
				// parse every music meta info.
				for (int i = 0; i < array.size(); ++i) {
					QJsonValue ele = array[i];
					//qDebug() << ele;
					if (ele.isObject()) {
						QJsonObject ele_object = ele.toObject();
						LocalMusicMetaInfo info = parseOneMusicInfo(ele_object);
						m_musicData.push_back(info);
					}
				}
			}
		}
	}
}

LocalMusicMetaInfo KLocalMusicManager::parseOneMusicInfo(QJsonObject& object)
{
	LocalMusicMetaInfo info;
	// path
	if (object.contains("path")) {
		QJsonValue value = object.value("path");
		if (value.isString()) {
			QString pathStr = value.toString();
			info.path = pathStr;
		}
	}
	if (object.contains("musicName")) {
		QJsonValue value = object.value("musicName");
		if (value.isString()) {
			QString musicNameStr = value.toString();
			info.musicName = musicNameStr;
		}
	}
	// albumName
	if (object.contains("albumName")) {
		QJsonValue value = object.value("albumName");
		if (value.isString()) {
			QString albumNameStr = value.toString();
			info.albumName = albumNameStr;
		}
	}
	// duration
	if (object.contains("duration")) {
		QJsonValue value = object.value("duration");
		if (value.isDouble()) {
			int duration = value.toVariant().toInt();
			info.duration = duration;
		}
	}
	// albumName
	if (object.contains("singer")) {
		QJsonValue value = object.value("singer");
		if (value.isString()) {
			QString singerStr = value.toString();
			info.singer = singerStr;
		}
	}

	return info;
}
