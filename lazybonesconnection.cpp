#include "lazybonesconnection.h"
#include "lazybonesprotocol.h"
#include <QThread>
#include <QWaitCondition>
#include <QDataStream>
#include <QSharedPointer>
#include "lazybonesclient.h"

class ProtocolSender : public QThread
{
public:
	ProtocolSender(LazybonesConnection *owner) : QThread(), m_owner(owner), m_stop(false)
	{
	}

	void run() Q_DECL_OVERRIDE
	{
		while(!m_stop)
		{
			QSharedPointer<LazybonesProtocol> protocol;
			m_lock.lock();
			if(m_owner->isValid() && m_queue.size() > 0)
			{
				protocol = m_queue.front();
				m_queue.removeFirst();
			}
			else
			{
				m_condition.wait(&m_lock);
			}
			m_lock.unlock();

			if(protocol)
			{
				// SEND IT!
				protocol->setFrom(m_owner->m_userId);
				protocol->send();
				if(!m_owner->waitForBytesWritten())
				{
					// ERROR
					m_owner->close();
				}
			}
		}
	}

	void add(QSharedPointer<LazybonesProtocol> protocol)
	{
		QMutexLocker lock(&m_lock);
		m_queue.push_back(protocol);
		m_condition.wakeAll();
	}

	void stop(void)
	{
		QMutexLocker lock(&m_lock);
		m_stop = true;
		m_condition.wakeAll();
	}

private:
	LazybonesConnection *m_owner;
	bool m_stop;
	QWaitCondition m_condition;
	QMutex m_lock;
	QList<QSharedPointer<LazybonesProtocol> > m_queue;
};

LazybonesConnection::LazybonesConnection(LazybonesClient *client) : QTcpSocket(), m_client(client), m_sender(NULL)
{
	startSender();

	QObject::connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	QObject::connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

LazybonesConnection::~LazybonesConnection(void)
{
	stopSender();
}

void LazybonesConnection::startSender(void)
{
	if(m_sender)
		stopSender();

	m_sender = new ProtocolSender(this);
	m_sender->start();
}

void LazybonesConnection::stopSender(void)
{
	if(!m_sender)
		return;

	m_sender->stop();
	m_sender->wait();
	delete m_sender;
	m_sender = NULL;
}


bool LazybonesConnection::connect(QString address, int port)
{
	connectToHost(address, port);
	bool res =	 waitForConnected();
	qDebug() << "CONNECT RESULT :" << res;
	return res;
}

void LazybonesConnection::join(QString channelId, QString userId, bool master)
{
	m_channelId = channelId;
	m_userId = userId;

	LazybonesProtocol::Header header;
	QByteArray payload;

	header.command = LazybonesProtocol::Join;
	header.from = userId;

	QDataStream stream(&payload, QIODevice::WriteOnly);
	LazybonesProtocol::writeString32(stream, channelId);
	stream << master;

	QSharedPointer<LazybonesProtocol> protocol = QSharedPointer<LazybonesProtocol>(new LazybonesProtocol(this, header, payload));
	m_sender->add(protocol);

	qDebug() << "join :" << channelId << userId << master << payload.length();
}

QSharedPointer<ChannelUserInfo> LazybonesConnection::findUser(const QString &userId)
{
	for(int i = 0; i < m_userList.size(); i++)
	{
		if(m_userList[i]->userId() == userId)
		{
			return m_userList[i];
		}
	}
	return QSharedPointer<ChannelUserInfo>();
}

void LazybonesConnection::removeUser(const QString &userId)
{
	for(int i = 0; i < m_userList.size(); i++)
	{
		if(m_userList[i]->userId() == userId)
		{
			m_userList.removeAt(i);
			return;
		}
	}
}

QString LazybonesConnection::masterUserId(void)
{
	for(int i = 0; i < m_userList.size(); i++)
	{
		if(m_userList[i]->isMaster())
		{
			return m_userList[i]->userId();
		}
	}
	return "";
}

void LazybonesConnection::sendRelayData(const LazybonesProtocol::Header &header, const QByteArray &payload)
{
	// TODO
	emit receivedRelayData(header, payload);
}

void LazybonesConnection::onReadyRead(void)
{
	if(!m_currentProtocol)
	{
		m_currentProtocol = QSharedPointer<LazybonesProtocol>(new LazybonesProtocol(this));
	}

	QByteArray buf = readAll();

	if(m_currentProtocol->parse(buf))
	{
		const QByteArray &payload = m_currentProtocol->payload();
		switch(m_currentProtocol->header().command)
		{
		case LazybonesProtocol::NotifyJoined:
		{
			int joinerCount;

			// parsing
			QDataStream stream(payload);
			stream >> joinerCount;
			for(int i = 0; i < joinerCount; i++)
			{
				QSharedPointer<ChannelUserInfo> userInfo = QSharedPointer<ChannelUserInfo>(new ChannelUserInfo());
				userInfo->loadFromDataStream(stream);
				m_userList.append(userInfo);
			}

			emit notifyJoined(m_currentProtocol->header().from);
			qDebug() << "notifyJoined :" << m_currentProtocol->header().from << joinerCount;
			break;
		}
		case LazybonesProtocol::NotifyLeft:
		{
			emit notifyLeft(m_currentProtocol->header().from);

			removeUser(m_currentProtocol->header().from);
			qDebug() << "notifyLeft :" << m_currentProtocol->header().from;
			break;
		}
		default:
		{
			emit receivedRelayData(m_currentProtocol->header(), payload);
			break;
		}
		}

		m_currentProtocol.clear();
	}
}

void LazybonesConnection::onDisconnected(void)
{

}
