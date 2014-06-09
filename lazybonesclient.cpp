#include "lazybonesclient.h"
#include "screenview.h"

LazybonesClient::LazybonesClient(bool master) : QObject(), m_serverPort(7000), m_master(master), m_connection(this), m_statsRecvBytes(0)
{
	m_imageProvider.start();
	m_imageProvider.setClient(this);

	QObject::connect(&m_connection, SIGNAL(receivedRelayData(LazybonesProtocol::Header,QByteArray)), this, SLOT(onReceivedRelayData(LazybonesProtocol::Header,QByteArray)));
}

LazybonesClient::~LazybonesClient(void)
{
	m_imageProvider.stop();
}

void LazybonesClient::connect(void)
{
	if(m_serverAddress.isEmpty() || m_serverPort <= 0)
	{
		return;
	}

	if(m_connection.connect(m_serverAddress, m_serverPort))
	{
		m_connection.join(m_channelId, m_userId, m_master);
	}
}

void LazybonesClient::addScreenView(ScreenView *view)
{
	removeScreenView(view);
	m_screenViews.push_back(view);
}

void LazybonesClient::removeScreenView(ScreenView *view)
{
	for(int i = 0; i < m_screenViews.size(); i++)
	{
		if(m_screenViews[i] == view)
		{
			m_screenViews.removeAt(i);
			break;
		}
	}
}

void LazybonesClient::draw(QSharedPointer<ImageTileData> tile)
{
	for(int i = 0; i <  m_screenViews.size(); i++)
	{
		m_screenViews[i]->draw(tile);
	}
}

void LazybonesClient::onReceivedRelayData(const LazybonesProtocol::Header &header, const QByteArray &payload)
{
	addStatsRecvBytes(payload.size());

	switch(header.command)
	{
	case LazybonesProtocol::ImageFrame:
		QDataStream stream(payload);
		QSharedPointer<ImageTileData> tile = QSharedPointer<ImageTileData>(new ImageTileData());
		tile->readStream(stream);

		draw(tile);
		break;
	}
}
