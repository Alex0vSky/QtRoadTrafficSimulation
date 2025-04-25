// src\Via\QuickItem\DraggableQQuickItem.h - add drag
namespace syscross::TraffModel::Via::QuickItem {
/**
 * QQuickItem with drag interaction capabilities
 */
class DraggableQQuickItem : public ZoomableQQuickItem {
	/// Current mouse cursor position
	QPoint m_mouseCur = { };

	/// Mouse movement delta since last event
	QPoint m_mouseDiff = { };

	/**
	 * @brief Handles mouse move events
	 *
	 * Calculates position delta:
	 * - Tracks cursor movement
	 * - Updates drag offset
	 * Between mousePressEvent and mouseReleaseEvent
	 *
	 * @param event Mouse event data
	 */
	void mouseMoveEvent(QMouseEvent *event) override {
		m_mouseDiff = event ->pos( ) - m_mouseCur;
	}

	/**
	 * @brief Handles mouse press events
	 *
	 * Initiates drag operation:
	 * - Sets closed hand cursor
	 * - Captures initial position
	 * Dont call base method @insp SO/handling-mouse-events-on-a-qquickitem
	 *
	 * @param event Mouse event data
	 */
    void mousePressEvent(QMouseEvent *event) override {
		setCursor( Qt::CursorShape::ClosedHandCursor );
		m_mouseCur = event ->pos( ) - m_mouseDiff;
    }

	/**
	 * @brief Handles mouse release events
	 *
	 * Completes drag operation:
	 * - Restores open hand cursor
	 *
	 * @param event Mouse event data
	 */
    void mouseReleaseEvent(QMouseEvent *event) override {
		setCursor( Qt::CursorShape::OpenHandCursor );
	}

public:
	/**
	 * @brief Constructs draggable item
	 * @param parent Optional parent QQuickItem
	 */
    explicit DraggableQQuickItem(QQuickItem *parent = nullptr) :
		ZoomableQQuickItem( parent )
	{
		setAcceptedMouseButtons( Qt::AllButtons );
		setCursor( Qt::CursorShape::OpenHandCursor );
	}

	/**
	 * @brief Applies drag transformation
	 *
	 * Modifies transformation matrix:
	 * - Translates by accumulated mouse delta
	 *
	 * @param transformNodeMatrix Matrix to modify
	 */
    void handleDrag(QMatrix4x4 *transformNodeMatrix) {
		transformNodeMatrix ->translate( m_mouseDiff.x( ), m_mouseDiff.y( ) );
	}
};
} // namespace syscross::TraffModel::Via::QuickItem
