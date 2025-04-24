// src\Via\GraphicsView\ZoomableQGraphicsView.h - add zoom
namespace syscross::TraffModel::Via::GraphicsView {
class ZoomableQGraphicsView : public BaseQGraphicsView {
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
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
    explicit ZoomableQGraphicsView(QMainWindow *parent) : 
		BaseQGraphicsView( parent )
	{
		setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
