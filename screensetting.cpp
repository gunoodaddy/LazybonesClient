#include "screensetting.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

ScreenSetting::ScreenSetting()
{
	QScreen *screen = QGuiApplication::primaryScreen();

	m_fps = 10;

	float sizeStep = 0.5;
	m_rect = QRect(0, 0, screen->size().width() / sizeStep, screen->size().height() / sizeStep);
}
