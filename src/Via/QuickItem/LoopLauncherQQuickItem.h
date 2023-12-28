// src\Via\QuickItem\LoopLauncherQQuickItem.h - render/game loop on `QTimer::timeout`
namespace syscross::TraffModel::Via::QuickItem {
class LoopLauncherQQuickItem : public DraggableQQuickItem {
	W_OBJECT( LoopLauncherQQuickItem ) //Q_OBJECT
	QTimer m_timer;
	virtual void loop() = 0;

public: 
    explicit LoopLauncherQQuickItem(QQuickItem *parent) : 
		DraggableQQuickItem( parent )
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
W_OBJECT_IMPL( LoopLauncherQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
