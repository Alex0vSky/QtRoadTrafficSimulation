// src\Via\GraphicsView\LoopLauncherQGraphicsView.h - render/game loop on `QTimer::timeout`
namespace syscross::TraffModel::Via::GraphicsView {
/**
 * QGraphicsView with built-in animation loop
 */
class LoopLauncherQGraphicsView : public DraggableQGraphicsView {
	/// Animation timer instance
	QTimer m_timer;

	/**
	 * @brief Executes main animation frame
	 *
	 * Pure virtual method requiring:
	 * - Scene updates
	 * - State progression
	 * - Visual refresh
	 */
	virtual void loop() = 0;

public:
	/**
	 * @brief Constructs loop-enabled view
	 * @param parent Parent main window
	 */
	explicit LoopLauncherQGraphicsView(QMainWindow *parent) :
		DraggableQGraphicsView( parent )
	{
		// @insp SO/qgraphicsview-doesnt-always-update
		QObject::connect( &m_timer, &QTimer::timeout
				, [this] {
					this ->loop( );
				}
			);
		m_timer.start( 50 );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
