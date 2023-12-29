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
    //掩码图（黑白色）
    QBitmap mask(size);
    QPainter painter(&mask);
    //Antialiasing：反走样（抗锯齿）
    painter.setRenderHint(QPainter::Antialiasing);
    //SmoothPixmapTransform：用来在对图片进行缩放时启用线性插值算法而不是最邻近算法
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //填充矩形
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    //画刷
    painter.setBrush(QColor(0, 0, 0));
    //绘制圆角矩形
    /*QPainter::drawRoundedRect
       (const QRectF &rect,
        qreal xRadius,
        qreal yRadius,
        Qt::SizeMode mode = Qt::AbsoluteSize)
    */
    painter.drawRoundedRect(0, 0, size.width(), size.height(), 150, 150);
    //自适应图片
    QPixmap image = src.scaled(size);
    //setMask：创建不规则窗口使用
    image.setMask(mask);
    return image;
}