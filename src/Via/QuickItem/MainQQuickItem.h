// src\Via\QuickItem\MainQQuickItem.h - main loop in separate thread
namespace syscross::TraffModel::Via::QuickItem {
class MainQQuickItem : public LoopLauncherQQuickItem {
	W_OBJECT( MainQQuickItem ) //Q_OBJECT

	QSGNode *m_carsNode = nullptr, *m_ligthsNode = nullptr, *m_roadsNode = nullptr;

	void addPolygon_(QSGNode* node, QPolygonF const& polygon, uint mode, QColor color) {
		QSGGeometry *geometry = new QSGGeometry( QSGGeometry::defaultAttributes_Point2D( ), polygon.size( ) );
		geometry ->setDrawingMode( mode );
		geometry ->setLineWidth( 1 );
		QSGGeometry::Point2D *vertices = geometry ->vertexDataAsPoint2D( );
		for ( auto const& point : polygon ) 
			(vertices++) ->set( point.x( ), point.y( ) );
		QSGGeometryNode *n = new QSGGeometryNode;
		n ->setGeometry( geometry );
		n ->setFlag( QSGNode::OwnsGeometry);
		QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
		material ->setColor( color );
		n ->setMaterial( material );
		n ->setFlag( QSGNode::OwnsMaterial);
		node ->appendChildNode( n );
	}
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override {
		if ( !oldNode ) {
			oldNode = new QSGNode( );
			oldNode ->appendChildNode( m_roadsNode = new QSGNode( ) );
			auto polygons = Sim::AllRoads::calc( );
			for ( QPolygonF const& polygon : polygons ) {
				// lower // QColor::fromRgb( 180, 180, 220 );
				addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawTriangleFan, Qt::gray );
				//// upper
				//addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawLineLoop, Qt::red );
			}

			Common::init( );

			oldNode ->appendChildNode( m_carsNode = new QSGNode( ) );
			oldNode ->appendChildNode( m_ligthsNode = new QSGNode( ) );
		}

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		while ( QSGNode* node = m_carsNode ->firstChild( ) ) 
			delete node;
		m_scener ->drawVehicles( [this](QPolygonF const& polygons) {
				addPolygon_( m_carsNode, polygons, QSGGeometry::DrawTriangleFan, Qt::blue );
				return nullptr;
			} );
		m_update ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_update ->outOfBoundsVehicles( &m_vehiclesOnMap );

		while ( QSGNode* node = m_ligthsNode ->firstChild( ) ) 
			delete node;
		m_scener ->drawSignals( [this, oldNode](QPolygonF const& polygons, QColor color) {
				addPolygon_( m_ligthsNode, polygons, QSGGeometry::DrawTriangleFan, color );
				return nullptr;
			} );
		m_update ->trafficSignals( t );

		QMatrix4x4 matrix = data ->transformNode ->matrix( );
		bool isDrag = DraggableQQuickItem::handleDrag( &matrix );
		bool isZoom = ZoomableQQuickItem::handleZoom( &matrix );
		if ( isDrag || isZoom )
			data ->transformNode ->setMatrix( matrix );
//		update( );
		return oldNode;
	} 
	void loop() override {
		update( );
	}

public:
	explicit MainQQuickItem(QQuickItem *parent = 0) : 
		LoopLauncherQQuickItem( parent )
	{}
};
W_OBJECT_IMPL( MainQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
