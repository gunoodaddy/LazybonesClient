#include "lazybonesprotocol.h"
#include "lazybonesconnection.h"

const static qint16 MAGIC_CODE = 0xcc;

LazybonesProtocol::LazybonesProtocol(LazybonesConnection *connection) : m_connection(connection)
  , m_dataStream(&m_tempBuffer, QIODevice::ReadWrite)
  , m_payloadSize(0)
  , m_parseState(LazybonesProtocol::HeaderState)
{
}

LazybonesProtocol::LazybonesProtocol(LazybonesConnection *connection, Header &header, const QByteArray &payload) : m_connection(connection)
  , m_header(header)
  , m_payload(payload)
  , m_payloadSize(payload.length())
  , m_parseState(LazybonesProtocol::CompleteState)
{
}

void LazybonesProtocol::send(void)
{
	QDataStream stream(m_connection);
	stream << MAGIC_CODE;
	stream << m_header.command;
	stream << m_header.calcLen();
	writeString32(stream, m_header.from);
	writeString32(stream, m_header.to);

	qDebug() << "Sent :" << m_header.command << m_payload.length();
	stream.writeBytes(m_payload.constData(), m_payload.length());
}

bool LazybonesProtocol::parse(const QByteArray &buffer)
{
	m_tempBuffer.append(buffer);

	switch(m_parseState)
	{
	case LazybonesProtocol::HeaderState:
		if(m_tempBuffer.length() >= Header::LENGTH())
		{
			m_dataStream >> m_header.magicCode;
			m_dataStream >> m_header.command;
			m_dataStream >> m_header.headerLen;
			m_parseState = LazybonesProtocol::HeaderExtensionState;
		}
		else
		{
			break;
		}
	case LazybonesProtocol::HeaderExtensionState:
		if(m_tempBuffer.length() >= m_header.headerLen)
		{
			readString32(m_dataStream, m_header.from);
			readString32(m_dataStream, m_header.to);

			m_dataStream >> m_payloadSize;

			m_parseState = LazybonesProtocol::PayloadState;
		}
		else
		{
			break;
		}
	case LazybonesProtocol::PayloadState:
		if(m_tempBuffer.length() >= Header::LENGTH() + m_payloadSize)
		{
			char *payload = new char[m_payloadSize];
			m_dataStream.readRawData(payload, m_payloadSize);
			m_payload.append(payload, m_payloadSize);
			delete [] payload;
			m_parseState = LazybonesProtocol::CompleteState;
		}
		else
		{
			return false;
		}
	case LazybonesProtocol::CompleteState:
		qDebug() << "Parse ok : magic =" << m_header.magicCode << ", command =" << m_header.command << ", length =" << m_payload.length();
		return true;
	}
	return false;
}
