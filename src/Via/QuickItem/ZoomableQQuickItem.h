// src\Via\QuickItem\ZoomableQQuickItem.h - add zoom
namespace syscross::TraffModel::Via::QuickItem {
class ZoomableQQuickItem : public BaseQQuickItem {
	using BaseQQuickItem::BaseQQuickItem;
	float m_zoom = 1.0f;
	QPoint m_point = { };
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
    void wheelEvent(QWheelEvent *pQEvent) override {
		float delta = pQEvent ->angleDelta( ).y( ) / 1800.0f;
		delta = roundf( delta * 100 ) / 100;
		m_zoom += delta;
		m_point = pQEvent ->position( ).toPoint( );
	}

public: 
	void handleZoom(QMatrix4x4 *transformNodeMatrix) {
		transformNodeMatrix ->translate( m_point.x( ), m_point.y( ) );
		transformNodeMatrix ->scale( m_zoom );
		transformNodeMatrix ->translate( -m_point.x( ), -m_point.y( ) );
	}
};
} // namespace syscross::TraffModel::Via::QuickItem
