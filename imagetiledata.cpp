#include "imagetiledata.h"

ImageTileData::ImageTileData()
{
}

void ImageTileData::readStream(QDataStream &stream)
{
	stream >> m_rect;
	stream >> m_image;
}

void ImageTileData::writeStream(QDataStream &stream)
{
	stream << m_rect;
	stream << m_image;
}

bool ImageTileData::equals(ImageTileData *rhs)
{
	return m_image == rhs->m_image;
}
