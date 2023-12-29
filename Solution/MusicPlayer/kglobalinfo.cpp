#include "kglobalinfo.h"
#include <QBitmap>
#include <QPainter>

QMap<QString, QString> qssTable = {
    {"playbtn", "./playbtn.qss"},
    {"suspendbtn", "./suspendbtn.qss"},
    {"volumenormal", "./volumenormal.qss"},
    {"volumesilence", "./volumesilence.qss"},
    {"seqmode", "./sequencemode.qss"},
    {"replaymode", "./replaymode.qss"},
    {"randommode", "./randommode.qss"}
};

QPixmap pixmapToRound(QPixmap& src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }
    QSize size(2 * radius, 2 * radius);
    //����ͼ���ڰ�ɫ��
    QBitmap mask(size);
    QPainter painter(&mask);
    //Antialiasing��������������ݣ�
    painter.setRenderHint(QPainter::Antialiasing);
    //SmoothPixmapTransform�������ڶ�ͼƬ��������ʱ�������Բ�ֵ�㷨���������ڽ��㷨
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //������
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    //��ˢ
    painter.setBrush(QColor(0, 0, 0));
    //����Բ�Ǿ���
    /*QPainter::drawRoundedRect
       (const QRectF &rect,
        qreal xRadius,
        qreal yRadius,
        Qt::SizeMode mode = Qt::AbsoluteSize)
    */
    painter.drawRoundedRect(0, 0, size.width(), size.height(), 150, 150);
    //����ӦͼƬ
    QPixmap image = src.scaled(size);
    //setMask�����������򴰿�ʹ��
    image.setMask(mask);
    return image;
}