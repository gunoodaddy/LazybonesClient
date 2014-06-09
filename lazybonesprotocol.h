#ifndef LAZYBONESPROTOCOL_H
#define LAZYBONESPROTOCOL_H

#include <QByteArray>
#include <QDataStream>

class LazybonesConnection;

class LazybonesProtocol
{
public:
	enum CommandType {
		Join = 1,
		NotifyJoined = 2,
		NotifyLeft = 3,
		//------ RELAY -------
		ImageFrame = 10,
	};

	enum ParseState {
		HeaderState,
		HeaderExtensionState,
		PayloadState,
		CompleteState,
	};

	class Header
	{
	public:
		static int LENGTH(void) { return 10; }
		qint32 calcLen(void) { return LENGTH() + 4 + from.length() + 4 + to.length(); }

		qint16 magicCode;	// 2
		qint32 headerLen;	// 4
		qint32 command;		// 4
		//--------------------------
		// Extension Header
		//--------------------------
		QString from;		// 4
		QString to;			// 4
	};

public:
	LazybonesProtocol(LazybonesConnection *connection);
	LazybonesProtocol(LazybonesConnection *connection, Header &header, const QByteArray &payload);

	void send(void);
	bool parse(const QByteArray &buffer);

	void setFrom(const QString &from) { m_header.from = from; }
	void setTo(const QString &to) { m_header.to = to; }

	const Header& header(void) { return m_header; }
	const QByteArray& payload(void) { return m_payload; }

	static int writeString32(QDataStream &stream, const QString &str)
	{
		QByteArray array = str.toUtf8();
		stream.writeBytes(array.constData(), array.size());
		return 4 + array.size();
	}

	static int readString32(QDataStream &stream, QString &str)
	{
		char *buf;
		uint len;
		stream.readBytes(buf, len);
		if(buf && len > 0)
		{
			str = QString::fromUtf8(buf, len);
			delete [] buf;
		}

		return 4 + len;
	}

private:


private:
	LazybonesConnection *m_connection;

	Header m_header;
	QByteArray m_payload;
	QByteArray m_tempBuffer;
	QDataStream m_dataStream;
	int m_payloadSize;
	ParseState m_parseState;
};

#endif // LAZYBONESPROTOCOL_H
