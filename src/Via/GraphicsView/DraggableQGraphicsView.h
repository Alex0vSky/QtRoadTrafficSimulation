// src\Via\GraphicsView\DraggableQGraphicsView.h - add drag
namespace syscross::TraffModel::Via::GraphicsView {
/**
 * QGraphicsView with drag interaction support
 */
class DraggableQGraphicsView : public ZoomableQGraphicsView {
	/// Viewport margin for auto-scaling
	const QPoint m_viewportTopleft = { -20, -20 };

	/// Viewport margin for auto-scaling
	const QPoint m_viewportBottomRight = { 20, 20 };

	/// Scene boundaries tracking
	QPointF m_topLeft, m_bottomRight;

	/**
	 * @brief Handles mouse move events
	 *
	 * Manages:
	 * - Drag viewport movement
	 * - Scene boundary checks
	 * - Auto-scene expansion
	 * @insp SO/allow-qgraphicsview-to-move-outside-scene
	 *
	 * @param event Mouse event data
	 */
	void mouseMoveEvent(QMouseEvent* event) override {
		QGraphicsView::mouseMoveEvent(event);
		if ( ( event ->buttons( ) & Qt::LeftButton ) && scene( ) )
			// If we are moveing with the left button down, update the scene to trigger autocompute
			scene()->update(mapToScene(rect()).boundingRect());
	}

	/**
	 * @brief Handles mouse press events
	 *
	 * Initiates:
	 * - Drag operation mode
	 * - Cursor change
	 *
	 * @param event Mouse event data
	 */
	void mousePressEvent(QMouseEvent* event) override {
		if (event->buttons() & Qt::LeftButton)
			// Set drag mode when left button is pressed
			setDragMode(QGraphicsView::ScrollHandDrag);
		QGraphicsView::mousePressEvent(event);
	}

	/**
	 * @brief Handles mouse release events
	 *
	 * Completes:
	 * - Drag operation
	 * - Cursor restoration
	 *
	 * @param event Mouse event data
	 */
	void mouseReleaseEvent(QMouseEvent* event) override {
		if (dragMode() & QGraphicsView::ScrollHandDrag)
			// Unset drag mode when left button is released
			setDragMode(QGraphicsView::NoDrag);
		QGraphicsView::mouseReleaseEvent(event);
	}

	/**
	 * @brief Adjusts scene boundaries dynamically
	 *
	 * Expands scene rect when:
	 * - Viewport approaches edges
	 * - New content appears
	 */
	void autocomputeSceneSize(const QList<QRectF>&) {
		if ( QGraphicsView::ScrollHandDrag != dragMode( ) )
			return;
		// Widget viewport recangle
		QRectF widget_rect_in_scene(
				mapToScene( m_viewportTopleft )
				, mapToScene( rect( ).bottomRight( ) + m_viewportBottomRight )
			);
		// Copy the new size from the old one
		m_topLeft = sceneRect().topLeft();
		m_bottomRight = sceneRect().bottomRight();

		// Check that the scene has a bigger limit in the top side
		if (sceneRect().top() > widget_rect_in_scene.top())
			m_topLeft.setY(widget_rect_in_scene.top());

		// Check that the scene has a bigger limit in the bottom side
		if (sceneRect().bottom() < widget_rect_in_scene.bottom())
			m_bottomRight.setY(widget_rect_in_scene.bottom());

		// Check that the scene has a bigger limit in the left side
		if (sceneRect().left() > widget_rect_in_scene.left())
			m_topLeft.setX(widget_rect_in_scene.left());

		// Check that the scene has a bigger limit in the right side
		if (sceneRect().right() < widget_rect_in_scene.right())
			m_bottomRight.setX(widget_rect_in_scene.right());

		// Set new scene size
		setSceneRect( QRectF( m_topLeft, m_bottomRight ) );
	}

public:
	/**
	 * @brief Constructs draggable view
	 * @param parent Parent main window
	 */
	explicit DraggableQGraphicsView(QMainWindow *parent) :
		ZoomableQGraphicsView( parent )
	{
		connect( scene( ), &QGraphicsScene::changed, this, &DraggableQGraphicsView::autocomputeSceneSize );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
