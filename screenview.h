#ifndef SCREENVIEW_H
#define SCREENVIEW_H

#include <QQuickPaintedItem>
#include "lazybonesclient.h"

class ImageTileData;

class ScreenView : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(LazybonesClient * client READ client WRITE setClient NOTIFY clientChanged)

public:
	explicit ScreenView(QQuickItem *parent = 0);
	virtual ~ScreenView(void);

	LazybonesClient * client(void) { return m_client; }
	void setClient(LazybonesClient *client);

	void draw(QSharedPointer<ImageTileData> tile);

	virtual void paint(QPainter *painter);

signals:
	void clientChanged(void);

public slots:
	void onClientDestroyed(QObject*);
	void onScreenImageSettingChanged(void);

private:
	LazybonesClient *m_client;
	QImage *m_image;
};

#endif // SCREENVIEW_H
