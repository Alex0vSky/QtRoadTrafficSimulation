// src\Via\QuickItem\ZoomableQQuickItem.h - add zoom
namespace syscross::TraffModel::Via::QuickItem {
/**
 * QQuickItem with zoom interaction capabilities
 */
class ZoomableQQuickItem : public BaseQQuickItem {
	using BaseQQuickItem::BaseQQuickItem;
	/// Current zoom scale factor (1.0 = 100%)
	float m_zoom = 1.0f;

	/// Last mouse position used as zoom pivot point
	QPoint m_point = { };

	/**
	 * @brief Handles mouse wheel zoom events
	 *
	 * Calculates zoom delta from wheel movement:
	 * - Normalizes wheel delta to zoom factor
	 * - Stores current mouse position as pivot
	 * - Smoothly interpolates between zoom levels
	 * @insp SO/zoom-functionality-using-qt
	 *
	 * @param pQEvent Mouse wheel event data
	 */
    void wheelEvent(QWheelEvent *pQEvent) override {
		float delta = pQEvent ->angleDelta( ).y( ) / 1800.0f;
		delta = roundf( delta * 100 ) / 100;
		m_zoom += delta;
		m_point = pQEvent ->position( ).toPoint( );
	}

public:
	/**
	 * @brief Applies zoom to transformation matrix
	 *
	 * Performs scale transformation:
	 * - Centers zoom at last mouse position
	 * - Maintains visual content stability
	 * - Preserves existing transformations
	 *
	 * @param transformNodeMatrix Matrix to modify
	 */
	void handleZoom(QMatrix4x4 *transformNodeMatrix) {
		transformNodeMatrix ->translate( m_point.x( ), m_point.y( ) );
		transformNodeMatrix ->scale( m_zoom );
		transformNodeMatrix ->translate( -m_point.x( ), -m_point.y( ) );
	}
};
} // namespace syscross::TraffModel::Via::QuickItem
