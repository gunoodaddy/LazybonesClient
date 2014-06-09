#ifndef SCREENIMAGEPROVIDER_H
#define SCREENIMAGEPROVIDER_H

#include "imagetiledata.h"

class ImageSender;
class LazybonesClient;

class ScreenImageProvider
{
public:
	ScreenImageProvider(void);

	void setClient(LazybonesClient *client) { m_client = client; }

	void start(void);
	void stop(void);
	void pause(void);
	void resume(void);

private:
	friend class ImageSender;
	ImageSender *m_sender;
	LazybonesClient *m_client;
};

#endif // SCREENIMAGEPROVIDER_H
