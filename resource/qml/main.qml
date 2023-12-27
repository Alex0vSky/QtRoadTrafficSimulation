import QtQuick 2.6
import QtQuick.Controls 2.1
import MyQQuickItem 1.0

ApplicationWindow {
	id: window
	width: 1000
	height: 630
	visible: true
	title: "TraffModel via QQuickItem, zoomable and moveable"

	MyQQuickItem {
		objectName: "TraffModelQQuickItem"
		anchors.fill: parent
	}		
}