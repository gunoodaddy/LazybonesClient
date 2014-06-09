#ifndef SCREENSETTING_H
#define SCREENSETTING_H

#include <QObject>
#include <QRect>

class ScreenSetting
{
public:
	ScreenSetting();

	int fps(void) const { return m_fps; }

	const QRect &rect(void) const { return m_rect; }

private:
	QRect m_rect;
	int m_fps;
};

#endif // SCREENSETTING_H
