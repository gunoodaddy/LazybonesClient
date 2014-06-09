import QtQuick 2.0
import gunoodaddy 1.0

Rectangle {
	ScreenView {
		anchors.fill: parent
		client: lazybonesClient
	}

	Text {
		anchors.right: parent.right
		anchors.top: parent.top
		color: "red"
		text: lazybonesClient.statsRecvBytes
	}
}
