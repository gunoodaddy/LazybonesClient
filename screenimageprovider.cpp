#include "screenimageprovider.h"
#include "lazybonesclient.h"
#include <QThread>
#include <QWaitCondition>
#include <QGuiApplication>
#include <QScreen>
#include <QImage>
#include <QDebug>
#include <QPixmap>

#define kDefaultImageSplitWidthCount 10
#define kDefaultImageSplitHeightCount 10

class ImageSender : public QThread
{
public:
	ImageSender(ScreenImageProvider *owner) : m_owner(owner), m_stopped(false), m_paused(false) { }

	void run() Q_DECL_OVERRIDE
	{
		while(!m_stopped)
		{
			m_lock.lock();
			if(m_paused)
			{
				m_condition.wait(&m_lock);
				m_paused = false;
			}

			m_condition.wait(&m_lock, 1000 / m_owner->m_client->screenSetting().fps());
			m_lock.unlock();

			splitImages();

			QVector<QSharedPointer<ImageTileData> > titleList = diffImage();

			sendImages(titleList);

			m_prevImages = m_currImages;
		}
	}

	QImage captureImage(void)
	{
		QScreen *screen = QGuiApplication::primaryScreen();
		QPixmap screenShotPixmap = screen->grabWindow(0);
		return screenShotPixmap.toImage();
	}

	void splitImages(void)
	{
		const ScreenSetting &setting = m_owner->m_client->screenSetting();

		QImage image = captureImage();
		image = image.scaled(setting.rect().size(), Qt::KeepAspectRatio);

		int stepW = image.width() / kDefaultImageSplitWidthCount;
		int stepH = image.height() / kDefaultImageSplitHeightCount;

		m_currImages.clear();

//		return;
//		// FOR TEST
//		{
//			QSharedPointer<ImageTileData> tile = QSharedPointer<ImageTileData>(new ImageTileData());
//			tile->setRect(QRect(0, 0, image.width(), image.height()));
//			tile->setImage(image);
//			m_currImages.append(tile);
//			return;
//		}

		for(int x = 0; x < image.width(); )
		{
			for(int y = 0; y < image.height(); )
			{
				QSharedPointer<ImageTileData> tile = QSharedPointer<ImageTileData>(new ImageTileData());

				int w = stepW;
				int h = stepH;

				if(x + w > image.width())
				{
					w = image.width() - x;
				}

				if(y + h > image.height())
				{
					h = image.height() - y;
				}

				tile->setRect(QRect(x, y, w, h));

				QImage qimage = image.copy(tile->rect());
				tile->setImage(qimage);
				y += stepH;

				m_currImages.append(tile);
			}

			x += stepW;
		}
	}

	QVector<QSharedPointer<ImageTileData> > diffImage(void)
	{
		QVector<QSharedPointer<ImageTileData> > list;

		if(m_currImages.size() == m_prevImages.size())
		{
			for(int i = 0; i < m_currImages.size(); i++ )
			{
				QSharedPointer<ImageTileData> prevTile = m_prevImages[i];
				QSharedPointer<ImageTileData> currTile = m_currImages[i];

				if(currTile->equals(prevTile.data()) == false)
				{
					list.append(currTile);
				}
			}
		}
		else
		{
			return m_currImages;
		}

		return list;
	}

	void sendImages(QVector<QSharedPointer<ImageTileData> > tiles)
	{
		//qDebug() << "sendImages called :" << tiles.size() << m_currImages.size();

		for(int i = 0; i < tiles.size(); i++ )
		{
			QSharedPointer<ImageTileData> tile = tiles[i];
			QByteArray payload;
			QDataStream stream(&payload, QIODevice::WriteOnly);
			tile->writeStream(stream);

			LazybonesProtocol::Header header;
			header.command = LazybonesProtocol::ImageFrame;

			m_owner->m_client->connection()->sendRelayData(header, payload);
		}
	}

	void stop(void)
	{
		m_stopped = true;
		m_condition.wakeAll();
	}

	void pause(void)
	{
		m_paused = true;
	}

	void resume(void)
	{
		m_paused = false;
		m_condition.wakeAll();
	}

private:
	ScreenImageProvider *m_owner;
	bool m_stopped;
	bool m_paused;
	QMutex m_lock;
	QWaitCondition m_condition;
	QVector<QSharedPointer<ImageTileData> > m_currImages;
	QVector<QSharedPointer<ImageTileData> > m_prevImages;
};

ScreenImageProvider::ScreenImageProvider(void) : m_sender(NULL)
{
}

void ScreenImageProvider::start(void)
{
	if(m_sender)
		stop();

	m_sender = new ImageSender(this);
	m_sender->start();
}

void ScreenImageProvider::pause(void)
{
	if(m_sender)
		m_sender->pause();
}

void ScreenImageProvider::resume(void)
{
	if(m_sender)
		m_sender->resume();
}

void ScreenImageProvider::stop(void)
{
	if(!m_sender)
		return;

	m_sender->stop();
	m_sender->wait();
	delete m_sender;
	m_sender = NULL;
}
