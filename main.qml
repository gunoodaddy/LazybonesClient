import QtQuick 2.2
import QtQuick.Controls 1.1
import gunoodaddy 1.0

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")

	Component.onCompleted: {
		//stackView.push(connectPage);
		stackView.push(screenViewPage);
	}

	menuBar: MenuBar {
		Menu {
			title: qsTr("File")
			MenuItem {
				text: qsTr("Exit")
				onTriggered: Qt.quit();
			}
		}
	}

	LazybonesClient {
		id: lazybonesClient
	}

	StackView {
		id: stackView
		delegate: StackViewDelegate {
			function transitionFinished(properties)
			{
				properties.exitItem.opacity = 1
			}

			pushTransition: StackViewTransition {
				PropertyAnimation {
					target: enterItem
					property: "opacity"
					from: 0
					to: 1
				}
				PropertyAnimation {
					target: exitItem
					property: "opacity"
					from: 1
					to: 0
				}
			}
		}
	}

	Component {
		id: connectPage
		ConnectPage {

		}
	}

	Component {
		id: screenViewPage
		ScreenViewPage {

		}
	}
}
