// src\Via\QuickPaintedItem\ZoomableQQuickPaintedItem.h - add zoom
namespace syscross::TraffModel::Via::QuickPaintedItem {
/**
 * QQuickPaintedItem with zoom interaction support
 */
class ZoomableQQuickPaintedItem : public BaseQQuickPaintedItem {
	using BaseQQuickPaintedItem::BaseQQuickPaintedItem;
	/// Minimum allowed zoom threshold
	static constexpr float c_deltaMinimal = 0.1f;

	/// Current zoom scale factor
	float m_delta = 1.0f;

	/// Mouse position for zoom center point
	QPoint m_point;

	/**
	 * @brief Handles mouse wheel zoom events
	 *
	 * Processes:
	 * - Wheel delta to zoom factor conversion
	 * - Pivot point preservation
	 * - Minimum zoom level enforcement
	 * @insp SO/zoom-functionality-using-qt
	 *
	 * @param event Mouse wheel event data
	 */
	void wheelEvent(QWheelEvent *pQEvent) override {
		m_delta += pQEvent ->angleDelta( ).y( ) / 1200.0f;
		if ( m_delta < c_deltaMinimal )
			m_delta = c_deltaMinimal;
		m_point = pQEvent ->position( ).toPoint( );
	}

public:
	/**
	 * @brief Applies zoom transformation
	 *
	 * Modifies painter state:
	 * - Centers zoom at last mouse position
	 * - Applies current scale factor
	 * - Maintains content stability
	 *
	 * @param painter Target QPainter instance
	 */
	void handleZoom(QPainter *painter) {
		// origin to spot
		painter ->translate( m_point.x( ), m_point.y( ) );
		painter ->scale( m_delta, m_delta );
		// spot to origin
		painter ->translate( -m_point.x( ), -m_point.y( ) );
	}
};
} // namespace syscross::TraffModel::Via::QuickPaintedItem
