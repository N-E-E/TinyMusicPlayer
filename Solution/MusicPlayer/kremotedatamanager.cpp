#include "kremotedatamanager.h"

#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextCodec>


KRemoteDataManager::KRemoteDataManager(QObject *parent)
	: QObject(parent)
{
	downloader = new KHttpDownloader(this);
	m_dir = new QDir();
}

KRemoteDataManager::~KRemoteDataManager()
{
	if (downloader) delete downloader;
	if (m_dir) delete m_dir;
}

void KRemoteDataManager::parseAll()
{
	for (auto rootMusicFolder : m_rootMusic) {
		QString structureFile = m_rootUrl + rootMusicFolder + "/" + "music.json";
		// download in bloking mode. TODO: change to unblocked mode.
		downloader->download(structureFile);
		// parse
		QByteArray arr = downloader->getArrayObj();
		parseMusicMetaInfoList(arr);
	}
}

void KRemoteDataManager::parseMusicMetaInfoList(QByteArray& byteArray)
{
	//QString str = QString::fromLocal8Bit(byteArray.data());

	QTextCodec* codec = QTextCodec::codecForName("utf-8");//指定QString的编码方式
	QString name = codec->toUnicode(byteArray);//nameArray可以是char*，可以是QByteArray

	QByteArray arr = name.toUtf8();
	qDebug() << arr.data();
	
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
						MetaInfo info = parseOneMusicInfo(ele_object);
						m_remoteMusicInfo.push_back(info);
					}
				}
			}
		}
	}
}

MetaInfo KRemoteDataManager::parseOneMusicInfo(QJsonObject& object)
{
	MetaInfo info;
	if (object.contains("musicName")) {
		QJsonValue value = object.value("musicName");
		if (value.isString()) {
			QString singerTitleStr = value.toString();
			QRegularExpression re("-");
			QStringList lines = singerTitleStr.split(re);
			info.singer = lines[0].trimmed();
			info.musicName = lines[1].trimmed();
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
	// path
	if (object.contains("path")) {
		QJsonValue value = object.value("path");
		if (value.isString()) {
			QString pathStr = value.toString();
			info.path = pathStr;
		}
	}
	// mp3 filename
	if (object.contains("mp3")) {
		QJsonValue value = object.value("mp3");
		if (value.isString()) {
			QString mp3Str = value.toString();
			info.musicFileName = mp3Str;
		}
	}
	// lyric filename
	if (object.contains("lyric")) {
		QJsonValue value = object.value("lyric");
		if (value.isString()) {
			QString lyricStr = value.toString();
			info.lyricFileName = lyricStr;
		}
	}
	// img filename
	if (object.contains("img")) {
		QJsonValue value = object.value("img");
		if (value.isString()) {
			QString imgStr = value.toString();
			info.imgFileName = imgStr;
		}
	}

	return info;
}

QString KRemoteDataManager::fetchResource(QString url, QString cachePath)
{
	QString cacheFolder = "./remote" + cachePath;
	if (!m_dir->exists(cacheFolder)) {
		m_dir->mkpath(cacheFolder);
	}
	
	// music download
	QFile cacheMusicFile(cacheFolder + "/music.mp3");
	if (!cacheMusicFile.open(QIODevice::ReadOnly)) {  // don't have cache
		downloader->download(url + "/music.mp3", cacheFolder + "/music.mp3");
	}
	else {
		cacheMusicFile.close();
	}

	// img download
	QFile cacheImgFile(cacheFolder + "/img.jpg");
	if (!cacheImgFile.open(QIODevice::ReadOnly)) {  // don't have cache
		downloader->download(url + "/img.jpg", cacheFolder + "/img.jpg");
	}
	else {
		cacheImgFile.close();
	}

	// lyric download
	QFile cacheLyricFile(cacheFolder + "/lyric.txt");
	if (!cacheLyricFile.open(QIODevice::ReadOnly)) {  // don't have cache
		downloader->download(url + "/lyric.txt", cacheFolder + "/lyric.txt");
	}
	else {
		cacheLyricFile.close();
	}

	return cacheFolder;

}
