#include "channeluserinfo.h"
#include "lazybonesprotocol.h"

ChannelUserInfo::ChannelUserInfo() : QObject(), m_master(false)
{
}

void ChannelUserInfo::loadFromDataStream(QDataStream &stream)
{
	LazybonesProtocol::readString32(stream, m_userId);
	stream >> m_master;
}
