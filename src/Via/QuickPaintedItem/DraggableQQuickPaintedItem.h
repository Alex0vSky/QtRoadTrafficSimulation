// src\Via\QuickPaintedItem\DraggableQQuickPaintedItem.h - add drag
namespace syscross::TraffModel::Via::QuickPaintedItem {
/**
 * QQuickPaintedItem with drag interaction support
 */
class DraggableQQuickPaintedItem : public ZoomableQQuickPaintedItem {
	/// Current mouse cursor position
	QPoint m_mouseCur = { };

	/// Mouse movement delta since last event
	QPoint m_mouseDiff = { };

	/**
	 * @brief Handles mouse move events for drag tracking
	 *
	 * Calculates position delta:
	 * - Updates drag offset vector
	 * - Preserves event for parent handling
	 * Between mousePressEvent and mouseReleaseEvent
	 *
	 * @param event Mouse event data
	 */
	void mouseMoveEvent(QMouseEvent* event) override {
		m_mouseDiff = event ->pos( ) - m_mouseCur;
	}

	/**
	 * @brief Initiates drag operation
	 *
	 * Configures:
	 * - Closed hand cursor
	 * - Initial position capture
	 * Dont call base method @insp SO/handling-mouse-events-on-a-qquickitem
	 *
	 * @param event Mouse press event
	 */
	void mousePressEvent(QMouseEvent* event) override {
		setCursor( Qt::CursorShape::ClosedHandCursor );
		m_mouseCur = event ->pos( ) - m_mouseDiff;
	}

	/**
	 * @brief Completes drag operation
	 *
	 * Restores:
	 * - Open hand cursor
	 * - Default event handling
	 *
	 * @param event Mouse release event
	 */
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

	/**
	 * @brief Applies drag transformation to painter
	 *
	 * Modifies painter state:
	 * - Translates by accumulated mouse delta
	 * - Preserves existing transformations
	 *
	 * @param painter Target QPainter instance
	 */
	void handleDrag(QPainter *painter) {
		// @insp SO/transform-coordinates-in-a-qquickpainteditem
		painter ->translate( m_mouseDiff );
	}
};
} // namespace syscross::TraffModel::Via::QuickPaintedItem
