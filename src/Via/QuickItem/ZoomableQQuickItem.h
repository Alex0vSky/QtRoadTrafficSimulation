// src\Via\QuickItem\ZoomableQQuickItem.h - add zoom
namespace syscross::TraffModel::Via::QuickItem {
class ZoomableQQuickItem : public BaseQQuickItem {
	W_OBJECT( ZoomableQQuickItem ) //Q_OBJECT
	static constexpr float c_zoomMinimal{ 0.01f };
	float m_delta = 0, m_zoom = 1.0f;
	QPoint m_point = { };
	bool m_bDeltaChanged = false;
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
    void wheelEvent(QWheelEvent *pQEvent) override {
		float delta = pQEvent ->angleDelta( ).y( ) / 1800.0f;
		m_delta = roundf( delta * 100 ) / 100;
//		qDebug( ) << "m_delta" << m_delta;

		m_zoom += m_delta;
		//if ( m_zoom < c_zoomMinimal )
		//	m_zoom = c_zoomMinimal;
//		qDebug( ) << "m_zoom" << m_zoom;
		m_point = pQEvent ->position( ).toPoint( );
		m_bDeltaChanged = true;
	}

public: 
    explicit ZoomableQQuickItem(QQuickItem *parent) : 
		BaseQQuickItem( parent )
	{}
	bool handleZoom(QMatrix4x4 *transformNodeMatrix) {
		//if ( !m_bDeltaChanged ) 
		//	return false;
		transformNodeMatrix ->translate( m_point.x( ), m_point.y( ) );

		//qDebug( ) << "(1.0f + m_delta)" << ( 1.0f + m_delta );
		//transformNodeMatrix ->scale( 1.0f + m_delta );
//		qDebug( ) << "m_zoom" << ( m_zoom );

		transformNodeMatrix ->scale( m_zoom );

		transformNodeMatrix ->translate( -m_point.x( ), -m_point.y( ) );
		m_bDeltaChanged = false;
		return true;
	}
	float getZoom() {
		return m_zoom;
	}
	float getDelta() {
		return m_delta;
	}
};
W_OBJECT_IMPL( ZoomableQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
