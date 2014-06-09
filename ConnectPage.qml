import QtQuick 2.0
import QtQuick.Controls 1.1

Item {
	Column {
		spacing: 5
		anchors.centerIn: parent
		TextField {
			id: addressField
			width: 200
			//text: "192.168.219.108"
			text: "127.0.0.1"
		}
		TextField {
			id: portField
			text: "7000"
			inputMethodHints: Qt.ImhDigitsOnly
		}
		TextField {
			id: channelField
			width: 200
			text: "DesktopTeam"
		}
		TextField {
			id: idField
			width: 200
			text: "gunoodaddy"
		}
		Button {
			text: "Connect"
			width: addressField.width
			height: 40
			onClicked: {
				lazybonesClient.serverAddress = addressField.text
				lazybonesClient.serverPort = portField.text
				lazybonesClient.channelId = channelField.text
				lazybonesClient.userId = idField.text
				lazybonesClient.connect();
			}
		}
	}
}
