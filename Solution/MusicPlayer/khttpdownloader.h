#ifndef __K_HTTP_DOWNLOADER_H__
#define __K_HTTP_DOWNLOADER_H__


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class KHttpDownloader  : public QObject
{
	Q_OBJECT

public:
	KHttpDownloader(QObject *parent);
	~KHttpDownloader();

	void download(QString url, QString saveFilePath = "");

	void save(QString filepath);

	inline QByteArray getArrayObj() {
		return m_byteArrayObject;
	}

private:
	QNetworkAccessManager* m_networkManager = nullptr;
	QNetworkReply* m_reply = nullptr;

	QByteArray m_byteArrayObject;

	QString m_saveFilePath;
};

#endif // !__K_HTTP_DOWNLOADER_H__
