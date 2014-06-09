#ifndef IMAGETILEDATA_H
#define IMAGETILEDATA_H

#include <QImage>

class ImageTileData
{
public:
	ImageTileData();

	void readStream(QDataStream &stream);
	void writeStream(QDataStream &stream);

	void setRect(const QRect &rect) { m_rect = rect; }
	void setImage(const QImage &image) { m_image = image; }

	const QRect &rect(void) { return m_rect; }
	const QImage &image(void) { return m_image; }

	bool equals(ImageTileData *rhs);

private:
	QImage m_image;
	QRect m_rect;
};

#endif // IMAGETILEDATA_H
