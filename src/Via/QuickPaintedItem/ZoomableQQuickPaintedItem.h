// src\Via\QuickPaintedItem\ZoomableQQuickPaintedItem.h - add zoom
namespace syscross::TraffModel::Via::QuickPaintedItem {
class ZoomableQQuickPaintedItem : public BaseQQuickPaintedItem {
	W_OBJECT( ZoomableQQuickPaintedItem ) //Q_OBJECT
	using BaseQQuickPaintedItem::BaseQQuickPaintedItem;
	static constexpr float c_deltaMinimal{ 0.1f };
	float m_delta = 1;
	QPoint m_point = { };
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
    void wheelEvent(QWheelEvent *pQEvent) override {
		m_delta += pQEvent ->angleDelta( ).y( ) / 1200.0f;
		if ( m_delta < c_deltaMinimal )
			m_delta = c_deltaMinimal;
		m_point = pQEvent ->position( ).toPoint( );
	}

public: 
	void handleZoom(QPainter *painter) {
		// origin to spot
		painter ->translate( m_point.x( ), m_point.y( ) );
		painter ->scale( m_delta, m_delta );
		// spot to origin
		painter ->translate( -m_point.x( ), -m_point.y( ) );
	}
};
W_OBJECT_IMPL( ZoomableQQuickPaintedItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickPaintedItem
