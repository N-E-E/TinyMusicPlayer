#include "kmusicmodule.h"

void KMusicModule::play(const QString& filename) {
    // reset info
    m_curTime = 0.0;
    m_duration = 0.0;
    m_curTimeStr = "00:00";
    m_durationStr = "00:00";

    m_decoder->open(filename);

    m_duration = m_decoder->duration();
    qint64 durationInSec = static_cast<qint64>(m_duration);
    QString durationMinuteStr, durationSecondStr;
    if (durationInSec / 60 < 10) {
        durationMinuteStr = "0" + QString::number(durationInSec / 60);
    }
    else {
        durationMinuteStr = QString::number(durationInSec / 60);
    }

    if (durationInSec % 60 < 10) {
        durationSecondStr = "0" + QString::number(durationInSec % 60);
    }
    else {
        durationSecondStr = QString::number(durationInSec % 60);

    }
    m_durationStr = durationMinuteStr + ":" + durationSecondStr;

    auto format = m_decoder->format();
    m_audioOutput->deleteLater();
    m_audioOutput = new QAudioOutput(m_decoder->format());
    m_audioOutput->setBufferSize(format.sampleRate() * format.sampleSize() / 16);  // choose a good size.

    m_audioDevice = m_audioOutput->start();
    m_audioOutput->setVolume(m_volume / qreal(100.0));
    //m_playing = true;
    //m_playState = PlayState::PlayingType;
    m_playTimer->start(100);
}

void KMusicModule::setVolume(int volume) {
    m_volume = volume;
    m_audioOutput->setVolume(volume / qreal(100.0));
}

void KMusicModule::setProgress(qreal ratio) {
    size_t idx = static_cast<size_t>(m_decoder->getBufferSize() * ratio);
    if (idx >= m_decoder->getBufferSize()) idx = m_decoder->getBufferSize() - 1;
    m_decoder->setPacketIdx(idx);
}

void KMusicModule::update() {
    //qDebug() << "update";
    while (m_audioBuffer.size() < m_audioOutput->bytesFree()) {
        AudioPacket packet = m_decoder->nextPacket();
        QByteArray data = packet.data;
        qreal currentTime = packet.time;
        // -0.1 to avoid errors
        if (currentTime >= m_decoder->duration() - 0.1 || (data.isEmpty() && qFuzzyIsNull(currentTime))) {
            m_progress = 1.0;
            //m_playing = false;
            //m_playState = PlayState::PlayOverType;
            m_playTimer->stop();

            m_curTime = m_duration;
            m_curTimeStr = m_durationStr;
            /*m_decoding = false;
            m_decoder->stop();*/
            /*emit finished();
            emit playingChanged();
            emit progressChanged();*/
            emit sigUpdateProgress();
            emit sigMusicPlayOver();
            return;
        }
        else {
            m_curTime = currentTime;
            qint64 curTimeInSec = static_cast<qint64>(currentTime);
            m_curTimeStr = QString::number(curTimeInSec / 60) + ":" + QString::number(curTimeInSec % 60);

            QString curTimeMinuteStr, curTimeSecondStr;
            if (curTimeInSec / 60 < 10) {
                curTimeMinuteStr = "0" + QString::number(curTimeInSec / 60);
            }
            else {
                curTimeMinuteStr = QString::number(curTimeInSec / 60);
            }

            if (curTimeInSec % 60 < 10) {
                curTimeSecondStr = "0" + QString::number(curTimeInSec % 60);
            }
            else {
                curTimeSecondStr = QString::number(curTimeInSec % 60);

            }
            m_curTimeStr = curTimeMinuteStr + ":" + curTimeSecondStr;

            m_progress = currentTime / m_decoder->duration();

            emit sigUpdateProgress();
        }

        if (data.isEmpty()) break;
        m_audioBuffer += data;
    }

    int readSize = m_audioOutput->periodSize();
    int chunks = m_audioBuffer.size() / readSize;
    while (chunks--) {
        QByteArray pcm = m_audioBuffer.mid(0, readSize);
        int size = pcm.size();
        m_audioBuffer.remove(0, size);

        if (size) m_audioDevice->write(pcm);
        if (size != readSize) break;
    }
}
