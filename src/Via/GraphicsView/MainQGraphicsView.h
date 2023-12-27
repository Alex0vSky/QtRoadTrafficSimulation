// src\Via\GraphicsView\MainQGraphicsView.h - main loop in main thread
namespace syscross::TraffModel::Via::GraphicsView {
class MainQGraphicsView final : public LoopLauncherQGraphicsView {
	using LoopLauncherQGraphicsView::LoopLauncherQGraphicsView;

	Sim::Road::roads_t m_roads;
	static const uint c_vehicleRate = 15; // 35;
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;
	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;
	uint m_vehiclesOnMap = 0;
	Timing m_timing;
	std::unique_ptr< Updater > m_update;
	std::unique_ptr< Scener > m_scener;
	std::vector< QGraphicsPolygonItem * > m_vehiclesItems, m_trafficSignalItems;
	//std::vector< std::unique_ptr< Sim::Vehicle > > m_singleVehicleObject; // tmp

	// clear previous
	void sceneItemsErase_(std::vector< QGraphicsPolygonItem * > *items) {
		if ( items ->empty( ) )
			return;
		auto it = std::remove_if( 
				items ->begin( ), items ->end( )
				, [this](QGraphicsPolygonItem *p) {
					return scene( ) ->removeItem( p ), delete p, true;
				}
			);
		items ->erase( it, items ->end( ) );
	}

	void loop() override {
		if ( !m_vehicleGenerator ) {
			// Scene static elements
			auto polygons = Sim::AllRoads::calc( );
			QRectF sceneRect;
			for ( auto const& polygon : polygons ) {
				auto *item = scene( ) ->addPolygon( polygon, { Qt::gray }, Qt::gray );
				sceneRect = sceneRect.united( item ->boundingRect( ) );
			}
			this ->setSceneRect( sceneRect );

			m_roads = Sim::AllRoads::get( );
			//// tmp, aka single-vehicle generator
			//auto W_R_S = Sim::AllRoads::W_R_S( );
			//std::vector< uint > path;
			////path.push_back( 0 ); path.insert( path.end( ), W_R_S.begin( ), W_R_S.end( ) ); path.push_back( 5 );
			//path.push_back( 2 ); path.push_back( 10 ); path.push_back( 4 );
			//m_singleVehicleObject.push_back( std::make_unique< Sim::Vehicle >( path ) );
			//auto &vehicle = m_singleVehicleObject[ 0 ];
			//int firstRoad = 0;
			//auto roadIndex = vehicle ->path( )[ firstRoad ];
			//m_roads[ roadIndex ].addVehicle( vehicle.get( ) );

			// TODO(alex): to separate class `Xxx`
			auto allPaths = Sim::AllRoads::getAllPaths( );
			// add_generator
			Sim::AllRoads::inboundRoads_t inboundRoads;
			// @from https://www.codeconvert.ai/python-to-c++-converter
			for ( auto const& path : allPaths ) {
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
		}

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		sceneItemsErase_( &m_vehiclesItems );
		m_vehiclesItems = m_scener ->drawVehicles( 
			[this](QPolygonF const& polygons) {
				return scene( ) ->addPolygon( polygons, { Qt::blue }, Qt::blue );
			} );

		m_update ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_update ->outOfBoundsVehicles( &m_vehiclesOnMap );

		sceneItemsErase_( &m_trafficSignalItems );
		m_trafficSignalItems = m_scener ->drawSignals( 
			[this](QPolygonF const& polygons, QColor color) {
				return scene( ) ->addPolygon( polygons, color, color );
			} );
		m_update ->trafficSignals( t );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
