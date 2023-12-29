import QtQuick 2.6
import QtQuick.Controls 2.1
import MainQQuickPaintedItem 1.0

ApplicationWindow {
	id: window
	width: 1000
	height: 630
	visible: true
	title: "TraffModel via QQuickPaintedItem, zoomable and moveable"

	MainQQuickPaintedItem {
		anchors.fill: parent
	}		
}