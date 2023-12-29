#ifndef __K_MUSIC_MODULE_H__
#define __K_MUSIC_MODULE_H__


#include "kaudiodecoder.h"
#include "kglobalinfo.h"

#include <QObject>
#include <QAudioOutput>
#include <QTimer>


class KMusicModule  : public QObject
{
	Q_OBJECT

public:
	KMusicModule(QObject *parent) : QObject(parent)
	{
		m_decoder = new KAudioDecoder(this);
		m_playTimer = new QTimer(this);
        connect(m_playTimer, &QTimer::timeout, this, &KMusicModule::update);
	}

	~KMusicModule() {
        if (m_decoder) delete m_decoder;
        if (m_playTimer) delete m_playTimer;
        if (m_audioOutput) delete m_audioOutput;
        if (m_audioDevice) delete m_audioDevice;
    }


    void play(const QString& filename);

    inline void suspend() {
        m_playTimer->stop();
    }

    inline void resume() {
        m_playTimer->start(100);
    }

    void setVolume(int volume);

    inline int getVolume() {
        return m_volume;
    }

    inline qreal getProgress() {
        return m_progress;
    }

    void setProgress(qreal ratio);

    inline QString getDurationStr() {
        return m_durationStr;
    }

    inline qreal getDuration() {
        return m_duration;
    }

    inline QString getCurTimeStr() {
        return m_curTimeStr;
    }

    inline qreal getCurTime() {
        return m_curTime;
    }

signals:
    void sigMusicPlayOver();
    void sigUpdateProgress();  // tell the player to update slider bar.

private slots:
    void update();

private:
	KAudioDecoder* m_decoder = nullptr;
	qreal m_progress = 0.0;
	int m_volume = 30;
	QByteArray m_audioBuffer = QByteArray();
	QTimer* m_playTimer = nullptr;
	QAudioOutput* m_audioOutput = nullptr;
	QIODevice* m_audioDevice = nullptr;
    //bool m_playing = false;
    //PlayState m_playState = PlayState::NoSelectedMusicType;

    qreal m_curTime = 0.0;
    qreal m_duration = 0.0;
    QString m_curTimeStr = "00:00";
    QString m_durationStr = "00:00";

};


#endif // !__K_MUSIC_MODULE_H__
