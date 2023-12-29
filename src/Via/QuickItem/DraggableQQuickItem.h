// src\Via\QuickItem\DraggableQQuickItem.h - add drag
namespace syscross::TraffModel::Via::QuickItem {
class DraggableQQuickItem : public ZoomableQQuickItem {
	W_OBJECT( DraggableQQuickItem ) //Q_OBJECT
	QPoint m_mouseCur = { }, m_mouseDiff = { };

	// Between mousePressEvent and mouseReleaseEvent
	void mouseMoveEvent(QMouseEvent* event) override { 
		m_mouseDiff = event ->pos( ) - m_mouseCur;
	}
	// Dont call base method @insp https://stackoverflow.com/questions/18864420/handling-mouse-events-on-a-qquickitem
    void mousePressEvent(QMouseEvent* event) override {
		setCursor( Qt::CursorShape::ClosedHandCursor );
		m_mouseCur = event ->pos( ) - m_mouseDiff;
    }
    void mouseReleaseEvent(QMouseEvent* event) override {
		setCursor( Qt::CursorShape::OpenHandCursor );
	}

public: 
    explicit DraggableQQuickItem(QQuickItem *parent) : 
		ZoomableQQuickItem( parent )
	{
		setAcceptedMouseButtons( Qt::AllButtons );
		setCursor( Qt::CursorShape::OpenHandCursor );
	}
    void handleDrag(QMatrix4x4 *transformNodeMatrix) {
		transformNodeMatrix ->translate( m_mouseDiff.x( ), m_mouseDiff.y( ) );
	}
};
W_OBJECT_IMPL( DraggableQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
