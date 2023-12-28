// src\Via\QuickItem\DraggableQQuickItem.h - add drag
namespace syscross::TraffModel::Via::QuickItem {
class DraggableQQuickItem : public ZoomableQQuickItem {
	W_OBJECT( DraggableQQuickItem ) //Q_OBJECT

	static const uint c_dragSpeed = 2;
	QCursor openHandCursor = Qt::CursorShape::OpenHandCursor;
	QCursor closedHandCursor = Qt::CursorShape::ClosedHandCursor;
	float m_xTransformNodeMatrix = 0, m_yTransformNodeMatrix = 0;
	int m_xMouse = 0, m_yMouse = 0;
	bool m_bDrag = false;

	// Between mousePressEvent and mouseReleaseEvent
	void mouseMoveEvent(QMouseEvent* event) override { 
		m_xTransformNodeMatrix = event ->x( ) - m_xMouse;
		m_yTransformNodeMatrix = event ->y( ) - m_yMouse;
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
	}
	// Dont call base method @insp https://stackoverflow.com/questions/18864420/handling-mouse-events-on-a-qquickitem
    void mousePressEvent(QMouseEvent* event) override {
		this ->setCursor( closedHandCursor );
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
		m_bDrag = true;
    }
    void mouseReleaseEvent(QMouseEvent* event) override {
		this ->setCursor( openHandCursor );
		m_bDrag = false;
	}

public: 
    explicit DraggableQQuickItem(QQuickItem *parent) : 
		ZoomableQQuickItem( parent )
	{
		setAcceptedMouseButtons( Qt::AllButtons );
		setFlag( QQuickItem::ItemAcceptsInputMethod, true );
		this ->setCursor( openHandCursor );
	}
    bool handleDrag(QMatrix4x4 *transformNodeMatrix) {
		if ( !m_bDrag )
			return false;
		transformNodeMatrix ->translate( 
				m_xTransformNodeMatrix * c_dragSpeed
				, m_yTransformNodeMatrix * c_dragSpeed 
			);
		return true;
	}
};
W_OBJECT_IMPL( DraggableQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
