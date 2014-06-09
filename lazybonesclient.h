#ifndef LAZYBONESCLIENT_H
#define LAZYBONESCLIENT_H

#include <QObject>
#include <QUrl>
#include "lazybonesconnection.h"
#include "screenimageprovider.h"
#include "screensetting.h"
class ScreenView;

class LazybonesClient : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged)
	Q_PROPERTY(qint16 serverPort READ serverPort WRITE setServerPort NOTIFY serverPortChanged)
	Q_PROPERTY(QString channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged)
	Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
	Q_PROPERTY(bool master READ master WRITE setMaster NOTIFY masterChanged)
	Q_PROPERTY(QUrl screenSource READ screenSource NOTIFY screenSourceChanged)
	Q_PROPERTY(qint64 statsRecvBytes READ statsRecvBytes NOTIFY statsRecvBytesChanged)

public:
	explicit LazybonesClient(bool master = false);
	virtual ~LazybonesClient(void);

	void addScreenView(ScreenView *view);
	void removeScreenView(ScreenView *view);

	const QString &serverAddress(void) { return m_serverAddress; }
	void setServerAddress(const QString &serverAddress) { m_serverAddress = serverAddress; emit serverAddressChanged(); }

	qint16 serverPort(void) { return m_serverPort; }
	void setServerPort(qint16 serverPort) { m_serverPort = serverPort; emit serverPortChanged(); }

	const QString &channelId(void) { return m_channelId; }
	void setChannelId(const QString &channelId) { m_channelId = channelId; emit channelIdChanged(); }

	const QString &userId(void) { return m_userId; }
	void setUserId(const QString &userId) { m_userId = userId; emit userIdChanged(); }

	bool master(void) { return m_master; }
	void setMaster(bool master) { m_master = master; emit masterChanged(); }

	const QUrl &screenSource(void) { return m_currentScreenSource; }
	void setScreenSource(const QUrl &screenSource) { m_currentScreenSource = screenSource; emit screenSourceChanged(); }

	Q_INVOKABLE void connect(void);

	LazybonesConnection *connection(void) { return &m_connection; }

	const ScreenSetting& screenSetting(void) { return m_screenSetting; }
	void draw(QSharedPointer<ImageTileData> tile);

	qint64 statsRecvBytes(void) { return m_statsRecvBytes; }

	void addStatsRecvBytes(qint64 recvBytes)
	{
		m_statsRecvBytes += recvBytes;
		emit statsRecvBytesChanged();
	}

protected slots:
	void onReceivedRelayData(const LazybonesProtocol::Header &header, const QByteArray &payload);

signals:
	void serverAddressChanged(void);
	void serverPortChanged(void);
	void channelIdChanged(void);
	void userIdChanged(void);
	void masterChanged(void);
	void screenSourceChanged(void);
	void screenSettingChanged(void);
	void statsRecvBytesChanged(void);

private:
	friend class LazybonesConnection;
	QString m_serverAddress;
	qint16 m_serverPort;
	QString m_channelId;
	QString m_userId;
	bool m_master;
	QUrl m_currentScreenSource;

	LazybonesConnection m_connection;
	ScreenImageProvider m_imageProvider;

	QVector<ScreenView *> m_screenViews;

	ScreenSetting m_screenSetting;

	qint64 m_statsRecvBytes;
};

#endif // LAZYBONESCLIENT_H
