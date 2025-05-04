// src\Via\GraphicsView\ZoomableQGraphicsView.h - add zoom
namespace syscross::TraffModel::Via::GraphicsView {
/**
 * QGraphicsView with zoom interaction capabilities
 */
class ZoomableQGraphicsView : public BaseQGraphicsView {
	/**
	 * @brief Handles mouse wheel zoom events
	 * 
	 * Processes:
	 * - Wheel delta to zoom factor conversion
	 * - Viewport-relative zoom centering
	 * - Smooth transformation application
	 * @insp SO/zoom-functionality-using-qt
	 * 
	 * @param pQEvent Mouse wheel event data
	 */
	void wheelEvent(QWheelEvent *pQEvent) override {
		// pos() -> virtual canvas
		QPointF point = pQEvent->position( );
		QPointF pos = mapToScene( point.toPoint( ) );
		// scale from wheel angle
		float delta = 1.0f + pQEvent->angleDelta().y() / 1200.0f;
		// modify transform matrix
		QTransform xform = transform();
		xform.translate(pos.x(), pos.y()); // origin to spot
		xform.scale(delta, delta); // scale
		xform.translate(-pos.x(), -pos.y()); // spot to origin
		setTransform(xform);
		// force update
		update( );
	}

public: 
	/**
	 * @brief Constructs zoomable view
	 * @param parent Parent main window
	 */
	explicit ZoomableQGraphicsView(QMainWindow *parent) : 
		BaseQGraphicsView( parent )
	{
		setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
