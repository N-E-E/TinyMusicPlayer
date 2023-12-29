#include "klyricmodule.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

KLyricModule::KLyricModule(QObject *parent)
	: QObject(parent)
{}

KLyricModule::~KLyricModule()
{}

void KLyricModule::open(const QString & filename)
{
    m_buffer.clear();
    m_curIdx = 0;
    readFile(filename);
}

bool KLyricModule::readFile(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = file.readAll();
    file.close();

    QRegularExpression re("\\\\r\\\\n");
    QStringList lines = content.split(re);

    for (auto line : lines) {
        analyse(line);
    }
    return true;
}

bool KLyricModule::analyse(const QString& line)
{
    if (line == NULL || line.isEmpty()) {
        //qDebug() << "thie line is empty!";
        return false;
    }
    QRegularExpression regularExpression("\\[(\\d+)?:(\\d+)?(\\.\\d+)?\\](.*)?");
    int index = 0;
    QRegularExpressionMatch match;
    match = regularExpression.match(line, index);
    if (match.hasMatch()) {
        qreal totalTime;
        totalTime = static_cast<qreal>(match.captured(1).toInt() * 60) +
            static_cast<qreal>(match.captured(2).toInt()) +
            static_cast<qreal>(match.captured(3).toDouble());
        QString currentText = QString::fromStdString(match.captured(4).toStdString());      /*   获取歌词文本*/
        m_buffer.push_back({totalTime, currentText});
        return true;
    }
    return false;
}

LyricGroup KLyricModule::match(qreal time)
{
    // if need to go back.
    while (m_curIdx > 0 && time < m_buffer[m_curIdx].time) {
        m_curIdx--;
    }

    while (m_curIdx < m_buffer.size() - 1 && time > m_buffer[m_curIdx + 1].time) {
        m_curIdx++;
    }

    /*if (m_curIdx < m_buffer.size() - 1) {
        
        if (time > m_buffer[m_curIdx + 1].time) {
            m_curIdx++;
        }
    }*/
    
    LyricGroup group;
    if (m_curIdx == 0) {
        group.prev = "";
        group.cur = m_buffer[0].lyric;
        group.next = m_buffer[1].lyric;
    }
    else if (m_curIdx == m_buffer.size() - 1) {
        group.prev = m_buffer[m_curIdx - 1].lyric;
        group.cur = m_buffer[m_curIdx].lyric;
        group.next = "";
    }
    else {
        group.prev = m_buffer[m_curIdx - 1].lyric;
        group.cur = m_buffer[m_curIdx].lyric;
        group.next = m_buffer[m_curIdx + 1].lyric;
    }

    return group;
}
