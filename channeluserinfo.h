#ifndef CHANNELUSERINFO_H
#define CHANNELUSERINFO_H

#include <QObject>
#include <QDataStream>

class ChannelUserInfo : public QObject
{
public:
	ChannelUserInfo();

	void loadFromDataStream(QDataStream &stream);
	const QString &userId(void) { return m_userId; }
	bool isMaster(void) { return m_master; }

private:
	QString m_userId;
	bool m_master;
};

#endif // CHANNELUSERINFO_H
