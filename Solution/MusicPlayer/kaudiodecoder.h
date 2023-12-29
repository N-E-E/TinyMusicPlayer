#ifndef __K_AUDIO_DECODER_H__
#define __K_AUDIO_DECODER_H__

#include "kaudiodecodercore.h"

#include <QObject>
#include <QAudioFormat>


class KAudioDecoder  : public QObject
{
	Q_OBJECT

public:
    KAudioDecoder(QObject* parent) : QObject(parent) {
        m_core = new KAudioDecoderFFMpeg();
    }

    ~KAudioDecoder() {
        if (m_core) {
            delete m_core;
        }
    }


    void open(const QString& filename);


    AudioPacket nextPacket();


    void setPacketIdx(size_t idx) {
        m_packetIdx = idx;
    }


    inline size_t getPacketIdx() {
        return m_packetIdx;
    }


    inline void resetPacketIdx() {
        m_packetIdx = 0;
    }


    inline size_t getBufferSize() {
        return m_core->m_buffer.size();
    }


    inline qreal duration() {
        return m_core->m_duration;
    }


    inline QString title() {
        return m_core->m_title;
    }


    inline QString singer() {
        return m_core->m_singer;
    }


    inline QString album() {
        return m_core->m_album;
    }


    inline QAudioFormat format() {
        return m_core->m_format;
    }


private:
    KAudioDecoderFFMpeg* m_core = nullptr;
    size_t m_packetIdx = 0;
};

#endif // !__K_AUDIO_DECODER_H__

