#ifndef __K_MUSIC_LIST_ITEM_H__
#define __K_MUSIC_LIST_ITEM_H__


#include <QWidget>
#include "ui_kmusiclistitem.h"

class KMusicListItem : public QWidget
{
	Q_OBJECT

public:
	KMusicListItem(QWidget *parent = nullptr);
	~KMusicListItem();

	void setData(const size_t& musicIdx, const QString& folderName, const QString& title, const QString& singer, const QString& album, const QString& duration);

signals:
	void clicked(const size_t& musicIdx);

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	Ui::KMusicListItemClass ui;
	QString m_folderName;
	size_t m_musicIdx;
};

#endif // !__K_MUSIC_LIST_ITEM_H__
