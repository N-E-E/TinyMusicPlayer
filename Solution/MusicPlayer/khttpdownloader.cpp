#include "khttpdownloader.h"

#include <QThread>
#include <QEventLoop>

KHttpDownloader::KHttpDownloader(QObject *parent)
	: QObject(parent)
{
	m_networkManager = new QNetworkAccessManager(this);
	
}

KHttpDownloader::~KHttpDownloader()
{
	if (m_reply) delete m_reply;
	if (m_networkManager) delete m_networkManager;
}

void KHttpDownloader::download(QString url, QString saveFilePath)
{
	m_saveFilePath = saveFilePath;
	m_byteArrayObject.clear();

	QUrl httpUrl = QUrl::fromUserInput(url);
	m_reply = m_networkManager->get(QNetworkRequest(httpUrl));

	QEventLoop eventLoop;
	connect(m_reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

	m_byteArrayObject = m_reply->readAll();
	m_reply->deleteLater();

	save(saveFilePath);
}

void KHttpDownloader::save(QString filepath)
{
	if (filepath == "") {
		return;
	}

	QFile saveFile(filepath);
	if (!saveFile.open(QIODevice::WriteOnly)) {
		return;
	}

	saveFile.write(m_byteArrayObject);
	saveFile.close();
}

