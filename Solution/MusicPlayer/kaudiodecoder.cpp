#include "kaudiodecoder.h"

void KAudioDecoder::open(const QString& filename)
{
    resetPacketIdx();
    m_core->openAndDecode(filename);
}

AudioPacket KAudioDecoder::nextPacket() {
    if (m_packetIdx == m_core->m_buffer.size()) {
        return m_core->m_buffer[m_packetIdx - 1];
    }
    return m_core->m_buffer[m_packetIdx++];
}





