// src\Via\GraphicsView\BaseQGraphicsView.h - something based
namespace syscross::TraffModel::Via::GraphicsView {
class BaseQGraphicsView : public QGraphicsView, public Common {
	W_OBJECT( BaseQGraphicsView ) //Q_OBJECT

public: 
    explicit BaseQGraphicsView(QMainWindow *parent) : 
		QGraphicsView( parent )
	{
		setScene( new QGraphicsScene( this ) );
		setRenderHints( QPainter::Antialiasing );
	}
    BaseQGraphicsView(const BaseQGraphicsView&) = delete;
    BaseQGraphicsView& operator=(const BaseQGraphicsView&) = delete;
};
W_OBJECT_IMPL( BaseQGraphicsView ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::GraphicsView
