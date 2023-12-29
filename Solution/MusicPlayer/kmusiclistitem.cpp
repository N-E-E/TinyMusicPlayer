#include "kmusiclistitem.h"

KMusicListItem::KMusicListItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
}

KMusicListItem::~KMusicListItem()
{}

void KMusicListItem::setData(const size_t& musicIdx, const QString& folderName, const QString & title, const QString & singer, const QString & album, const QString & duration)
{
	m_folderName = folderName;
	m_musicIdx = musicIdx;
	ui.m_title->setText(title);
	ui.m_singerAndAlbum->setText(singer + "-" + album);
	ui.m_duration->setText(duration);
}

void KMusicListItem::mouseReleaseEvent(QMouseEvent* event)
{
	emit clicked(m_musicIdx);
}
