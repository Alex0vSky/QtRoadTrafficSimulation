// src\Via\QuickItem\ZoomableQQuickItem.h - add zoom
namespace syscross::TraffModel::Via::QuickItem {
class ZoomableQQuickItem : public BaseQQuickItem {
	W_OBJECT( ZoomableQQuickItem ) //Q_OBJECT
	float m_delta = 1;
	QPoint m_point;
	bool m_bDeltaChanged = false;
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
    void wheelEvent(QWheelEvent *pQEvent) override {
		QQuickItem::wheelEvent( pQEvent );
		m_delta = 1.0f + pQEvent ->angleDelta( ).y( ) / 800.0f;
		m_point = pQEvent ->position( ).toPoint( );
		m_bDeltaChanged = true;
	}

public: 
    explicit ZoomableQQuickItem(QQuickItem *parent) : 
		BaseQQuickItem( parent )
	{}
	bool handleZoom(QMatrix4x4 *transformNodeMatrix) {
		if ( !m_bDeltaChanged ) 
			return false;
		transformNodeMatrix ->translate( m_point.x( ), m_point.y( ) );
		transformNodeMatrix ->scale( m_delta );
		transformNodeMatrix ->translate( -m_point.x( ), -m_point.y( ) );
		m_bDeltaChanged = false;
		return true;
	}
};
W_OBJECT_IMPL( ZoomableQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
