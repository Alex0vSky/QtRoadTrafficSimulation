// src\Via\QuickPaintedItem\DraggableQQuickPaintedItem.h - add drag
namespace syscross::TraffModel::Via::QuickPaintedItem {
class DraggableQQuickPaintedItem : public ZoomableQQuickPaintedItem {
	W_OBJECT( DraggableQQuickPaintedItem ) //Q_OBJECT
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
    explicit DraggableQQuickPaintedItem(QQuickItem *parent = nullptr) : 
		ZoomableQQuickPaintedItem( parent )
	{
		setAcceptedMouseButtons( Qt::AllButtons );
		setCursor( Qt::CursorShape::OpenHandCursor );
	}
    void handleDrag(QPainter *painter) {
		// @insp https://stackoverflow.com/questions/27233446/transform-coordinates-in-a-qquickpainteditem
		painter ->translate( m_mouseDiff );
	}
};
W_OBJECT_IMPL( DraggableQQuickPaintedItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickPaintedItem
