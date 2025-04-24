// src\Via\GraphicsView\LoopLauncherQGraphicsView.h - render/game loop on `QTimer::timeout`
namespace syscross::TraffModel::Via::GraphicsView {
class LoopLauncherQGraphicsView : public DraggableQGraphicsView {
	QTimer m_timer;
	virtual void loop() = 0;

public: 
    explicit LoopLauncherQGraphicsView(QMainWindow *parent) : 
		DraggableQGraphicsView( parent )
	{
		// @insp https://stackoverflow.com/questions/28728820/qgraphicsview-doesnt-always-update
		QObject::connect( &m_timer, &QTimer::timeout
				, [this] { 
					this ->loop( );
				}
			);
		m_timer.start( 50 );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
