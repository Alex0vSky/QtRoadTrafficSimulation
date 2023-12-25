import QtQuick 2.6
import QtQuick.Controls 2.1
import MyQQuickItem 1.0

ApplicationWindow {
	id: window
	width: 1000
	height: 630
	visible: true
	title: "Qt Quick Controls 2, zoomable and moveable"

	MyQQuickItem {
		anchors.fill: parent
	}		
}