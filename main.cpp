#include <QApplication>
#include <QQmlApplicationEngine>
#include "lazybonesclient.h"
#include "screenview.h"
#include <qqml.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	qRegisterMetaType<LazybonesProtocol::Header>("LazybonesProtocol::Header");
	qmlRegisterType<LazybonesClient>("gunoodaddy", 1, 0, "LazybonesClient");
	qmlRegisterType<ScreenView>("gunoodaddy", 1, 0, "ScreenView");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
