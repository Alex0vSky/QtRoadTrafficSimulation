// src\Via\QuickItem\MainQQuickItem.h - main loop in separate thread
namespace syscross::TraffModel::Via::QuickItem {
// not final 'QQmlPrivate::QQmlElement<T>': cannot inherit from 'MainQQuickItem' as it has been declared as 'final'
class MainQQuickItem : public LoopLauncherQQuickItem {
	W_OBJECT( MainQQuickItem ) //Q_OBJECT

	Sim::Road::roads_t m_roads;
	static const uint c_vehicleRate = 15; // 35;
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;
	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;
	uint m_vehiclesOnMap = 0;
	Timing m_timing;
	std::unique_ptr< Updater > m_update;
	std::unique_ptr< Scener > m_scener;

	QSGNode *m_carsNode = nullptr, *m_ligthsNode = nullptr, *m_roadsNode = nullptr;

	void addPolygon_(QSGNode* node, QPolygonF const& polygon, uint mode, QColor color) {
		const QSGGeometry::AttributeSet &attribs = QSGGeometry::defaultAttributes_Point2D( );
		QSGGeometry *geometry = new QSGGeometry( attribs, polygon.size( ) );
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
			QColor colorRed = QColor::fromRgb( 255, 0, 0 );
			QColor colorGrey = QColor::fromRgb( 180, 180, 220 );
//static const int countPoints = 5; vertices[0].set( -10, 10 ), vertices[1].set( 10, 50 ), vertices[2].set( 30, 70 ), vertices[3].set( 60, 50 ), vertices[4].set( 50, 10 );
			m_roadsNode = new QSGNode( );
			oldNode ->appendChildNode( m_roadsNode );
			auto polygons = Sim::AllRoads::calc( );
			for ( QPolygonF const& polygon : polygons ) {
				// lower 
				addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawTriangleFan, colorGrey );
				//// upper
				//addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawLineLoop, colorRed );
			}

			m_roads = Sim::AllRoads::get( );
			// TODO(alex): to separate class `Xxx`
			auto allPaths = Sim::AllRoads::getAllPaths( );
			// add_generator
			Sim::AllRoads::inboundRoads_t inboundRoads;
			// @from https://www.codeconvert.ai/python-to-c++-converter
			for ( auto &path : allPaths ) {
				uint road_index = path.second[ 0 ];
				inboundRoads.insert( { road_index, &m_roads[ road_index ] } );
			}
			m_vehicleGenerator = std::make_unique< Sim::VehicleGenerator >( 
				c_vehicleRate, allPaths, inboundRoads );

			// add_traffic_signal
			Sim::Road::TrafficSignal::signalRoads_t signalRoads;
			Sim::AllRoads::signalIdxRoads_t signalIdxRoads = 
				Sim::AllRoads::getSignalIdxRoads( );
			for ( auto const& pair : signalIdxRoads ) 
				signalRoads.push_back( { &m_roads[ pair[ 0 ] ], &m_roads[ pair[ 1 ] ] } );
			m_trafficSignal = std::make_unique< Sim::Road::TrafficSignal >( 
				signalRoads );

			m_update = std::make_unique< Updater >( &m_roads, m_trafficSignal.get( ) );
			m_scener = std::make_unique< Scener >( &m_roads, m_trafficSignal.get( ) );

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

		QMatrix4x4 transformNodeMatrix = data ->transformNode ->matrix( );
		bool isDrag = DraggableQQuickItem::handleDrag( &transformNodeMatrix );
		bool isZoom = ZoomableQQuickItem::handleZoom( &transformNodeMatrix );
		if ( isDrag || isZoom )
			data ->transformNode ->setMatrix( transformNodeMatrix );
//		update( );
		return oldNode;
	} 
	void loop() override {
		update( );
	}

public:
	explicit MainQQuickItem(QQuickItem *parent = 0) : 
		LoopLauncherQQuickItem( parent )
	{
		setFlag( QQuickItem::ItemHasContents, true );
	}
};
W_OBJECT_IMPL( MainQQuickItem ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::QuickItem
