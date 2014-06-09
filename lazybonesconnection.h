#ifndef LAZYBONESSESSION_H
#define LAZYBONESSESSION_H

#include <QTcpSocket>
#include <QSharedPointer>
#include "lazybonesprotocol.h"
#include "channeluserinfo.h"

class ProtocolSender;
class LazybonesClient;

class LazybonesConnection : public QTcpSocket
{
	Q_OBJECT
public:
	LazybonesConnection(LazybonesClient *client);
	virtual ~LazybonesConnection(void);

	bool connect(QString address, int port);
	void join(QString channelId, QString userId, bool master);
	QSharedPointer<ChannelUserInfo> findUser(const QString &userId);
	void removeUser(const QString &userId);
	void sendRelayData(const LazybonesProtocol::Header &header, const QByteArray &payload);
	QString masterUserId(void);

private:
	void startSender(void);
	void stopSender(void);

signals:
	void notifyJoined(QString userId);
	void notifyLeft(QString userId);
	void receivedRelayData(const LazybonesProtocol::Header &header, const QByteArray &payload);

private slots:
	void onReadyRead(void);
	void onDisconnected(void);

private:
	friend class ProtocolSender;

	LazybonesClient *m_client;
	ProtocolSender *m_sender;
	QString m_userId;
	QString m_channelId;
	QSharedPointer<LazybonesProtocol> m_currentProtocol;
	QVector<QSharedPointer<ChannelUserInfo> > m_userList;
};

#endif // LAZYBONESSESSION_H
