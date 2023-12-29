#ifndef __K_LYRIC_MODULE_H__
#define __K_LYRIC_MODULE_H__


#include <QObject>
#include <QVector>

class lyricPacket {
public:
	qreal time;
	QString lyric;
};

class LyricGroup {
public:
	QString prev;
	QString cur;
	QString next;
};

class KLyricModule  : public QObject
{
	Q_OBJECT

public:
	KLyricModule(QObject *parent);
	~KLyricModule();

	void open(const QString& filename);

	bool readFile(const QString& filename);
	bool analyse(const QString& line);

	LyricGroup match(qreal time);

private:
	QVector<lyricPacket> m_buffer;
	size_t m_curIdx = 0;
};

#endif // !__K_LYRIC_MODULE_H__
