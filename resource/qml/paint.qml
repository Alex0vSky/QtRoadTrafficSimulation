import QtQuick 2.6
import QtQuick.Controls 2.1
import MyQQuickPaintedItem 1.0

ApplicationWindow {
	id: window
	width: 1000
	height: 630
	visible: true
	title: "Qt Quick Controls 2 via QQuickPaintedItem"

	MyQQuickPaintedItem {
		anchors.fill: parent
	}		
}