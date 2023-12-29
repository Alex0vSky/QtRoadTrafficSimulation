// src\Via\QuickPaintedItem\LoopLauncherQQuickPaintedItem.h - render/game loop on `QTimer::timeout`
namespace syscross::TraffModel::Via::QuickPaintedItem {
class LoopLauncherQQuickPaintedItem : public DraggableQQuickPaintedItem {
	W_OBJECT( LoopLauncherQQuickPaintedItem ) //Q_OBJECT
	QTimer m_timer;
	virtual void loop() = 0;

public: 
    explicit LoopLauncherQQuickPaintedItem(QQuickItem *parent = nullptr) : 
		DraggableQQuickPaintedItem( parent )
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
W_OBJECT_IMPL( LoopLauncherQQuickPaintedItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickPaintedItem
