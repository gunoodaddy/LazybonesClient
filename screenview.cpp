#include "screenview.h"
#include "imagetiledata.h"
#include <QPainter>

ScreenView::ScreenView(QQuickItem *parent) :
	QQuickPaintedItem(parent), m_client(NULL), m_image(NULL)
{
}

ScreenView::~ScreenView(void)
{
	if(m_client)
		m_client->removeScreenView(this);
}

void ScreenView::setClient(LazybonesClient *client)
{
	if(m_client)
	{
		m_client->removeScreenView(this);
		disconnect(m_client, SIGNAL(destroyed(QObject*)), this, SLOT(onClientDestroyed(QObject*)));
		disconnect(client, SIGNAL(screenSettingChanged()), this, SLOT(onScreenImageSettingChanged()));
	}

	client->addScreenView(this);
	connect(client, SIGNAL(destroyed(QObject*)), this, SLOT(onClientDestroyed(QObject*)));
	connect(client, SIGNAL(screenSettingChanged()), this, SLOT(onScreenImageSettingChanged()));

	m_client = client;

	onScreenImageSettingChanged();

	emit clientChanged();
}

void ScreenView::onClientDestroyed(QObject*)
{
	m_client = NULL;
}

void ScreenView::onScreenImageSettingChanged(void)
{
	const ScreenSetting &setting = m_client->screenSetting();

	if(m_image)
		delete m_image;

	m_image = new QImage(setting.rect().size(), QImage::Format_RGB32);
}

void ScreenView::draw(QSharedPointer<ImageTileData> tile)
{
	QPainter painter(m_image);
	painter.drawImage(tile->rect().topLeft(), tile->image());
	update(tile->rect());
}

void ScreenView::paint(QPainter *painter)
{
	if(!m_image)
		return;

	painter->drawImage(0, 0, *m_image);
}
