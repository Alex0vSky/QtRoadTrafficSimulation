// src\viaQGraphicsView.h - render via QGraphicsView, render in main thread
namespace syscross::TraffModel::Via::GraphicsView {
class MainQGraphicsView : public Via::GraphicsView::DraggableQGraphicsView {
	QTimer m_timer;
	typedef std::chrono::steady_clock clock_t;
	static constexpr auto now = clock_t::now;
	const clock_t::time_point c_dtZero;
	clock_t::time_point m_dtLast;

	std::vector< QGraphicsPolygonItem * > m_vehiclesItems, m_trafficSignalItems;
	std::unique_ptr< Sim::Draw > m_draw;

	Sim::Road::roads_t m_roads;

	static const uint c_vehicleRate = 65; // 35;
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;

	std::vector< std::unique_ptr< Sim::Vehicle > > m_singleVehicleObject; // tmp
	std::vector< Sim::Vehicle > m_vehiclesObjects2;
	uint n_vehicles_generated = 0, n_vehicles_on_map = 0;

	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;
	std::chrono::seconds m_timeToReswitch{ 5 };
	clock_t::time_point m_dtSwitchLast;


	void draw_vehicles() {
#pragma region clear previous
		auto it = std::remove_if( this ->m_vehiclesItems.begin( ), this ->m_vehiclesItems.end( )
				, [this](QGraphicsPolygonItem *p) {
					return this ->scene( ) ->removeItem( p ), delete p, true;
				}
			);
		this ->m_vehiclesItems.erase( it, this ->m_vehiclesItems.end( ) );
#pragma endregion
		QColor colorBlue = QColor::fromRgb( 0, 0, 255 );
		QBrush brush( colorBlue, Qt::SolidPattern );
		QPen pen( colorBlue );
		for ( auto & road : this ->m_roads ) {
			Sim::vehicles_t const& vehicles = road.getVehicles( );
			for ( auto & vehicle : vehicles ) {
				qreal l = vehicle ->length( ), h = vehicle ->width( );
				qreal sin = road.angle_sin( ), cos = road.angle_cos( );
				qreal x = road.start( ).x( ) + cos * vehicle ->x( );
				qreal y = road.start( ).y( ) + sin * vehicle ->x( );
				auto points = this ->m_draw ->rotated_box( { x, y }, { l, h }, cos, sin, true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				auto vehicleItem = this ->scene( ) ->addPolygon( polygon, pen, brush );
				this ->m_vehiclesItems.push_back( vehicleItem );
			}
		}
	}
	
	//// TODO(alex): to separate class `Timing`
	//timer_t m_t = 0;
	//qreal m_speed = 3;
	//timer_t m_tSpeeded = 0;
	//timer_t fixDeltaT() { 
	//	auto timePoint = now( );
	//	auto durationFrame = std::chrono::duration_cast
	//		< std::chrono::microseconds >
	//		( timePoint - this ->m_dtLast );
	//	this ->m_dtLast = timePoint;
	//	auto xxxInSecond = std::chrono::duration_cast
	//		< std::chrono::microseconds >
	//		( std::chrono::seconds{ 1 } ).count( );
	//	return static_cast< timer_t >( durationFrame.count( ) ) / xxxInSecond;
	//}
	Timing m_timing;

	void animation() {
		//qDebug( ) << "animation";
		//auto scene = this ->scene( );

		if ( !m_draw ) {
			m_draw = std::make_unique< Sim::Draw >( width( ), height( ) );

			// Scene static elements
			QColor colorGrey = QColor::fromRgb( 180, 180, 220 );
			QColor colorRed = QColor::fromRgb( 255, 0, 0 );
			QBrush brush_( colorGrey, Qt::SolidPattern );
			QPen pen_( brush_, 1 );
			//pen_.setColor( colorRed );
			auto polygons = Sim::AllRoads::calc( width( ), height( ) );
			QRectF sceneRect;
			for ( auto const& polygon : polygons ) {
				auto *item = scene( ) ->addPolygon( polygon, pen_, brush_ );
				sceneRect = sceneRect.united( item ->boundingRect( ) );
			}
			this ->setSceneRect( sceneRect );

			m_roads = Sim::AllRoads::get( );

			//// tmp, aka vehicle generator
			//auto W_R_S = Sim::AllRoads::W_R_S( );
			//std::vector< uint > path;
			////path.push_back( 0 );
			////path.insert( path.end( ), W_R_S.begin( ), W_R_S.end( ) );
			////path.push_back( 5 );
			//path.push_back( 2 );
			//path.push_back( 10 );
			//path.push_back( 4 );
			//m_singleVehicleObject.push_back( std::make_unique< Sim::Vehicle >( path ) );
			//auto &vehicle = m_singleVehicleObject[ 0 ];
			//int firstRoad = 0;
			//auto roadIndex = vehicle ->path( )[ firstRoad ];
			//m_roads[ roadIndex ].addVehicle( vehicle.get( ) );

			auto allPaths = Sim::AllRoads::getAllPaths( );
			// add_generator
			Sim::AllRoads::inboundRoads_t inboundRoads;
			// @from https://www.codeconvert.ai/python-to-c++-converter
			for ( auto &path : allPaths ) {
				uint weight = path.first;
				uint road_index = path.second[ 0 ];
				inboundRoads.insert( { road_index, &m_roads[ road_index ] } );
			}
			m_vehicleGenerator = std::make_unique< Sim::VehicleGenerator >( 
				c_vehicleRate, allPaths, inboundRoads );

			Sim::Road::TrafficSignal::signalRoads_t signalRoads;
			Sim::AllRoads::signalIdxRoads_t signalIdxRoads = 
				Sim::AllRoads::getSignalIdxRoads( );
			for ( auto const& pair : signalIdxRoads ) 
				signalRoads.push_back( { &m_roads[ pair[ 0 ] ], &m_roads[ pair[ 1 ] ] } );
			m_trafficSignal = std::make_unique< Sim::Road::TrafficSignal >( 
				signalRoads );
		}

		auto measurer = m_timing.createAutoMeasurer( );
//		timer_t dt;
//		// OR measured
//		dt = measurer.measuredDeltaT( );
//		// OR fixed for smooth moving
////		dt = m_timing.fixedDeltaT( );
//		timer_t dtSpeeded = dt * measurer.getSpeed( );
//		timer_t t = measurer.getTimeSpeeded( );

		auto [ t, dtSpeeded ] = measurer.getMeasured( );
		//auto [ t, dtSpeeded ] = measurer.getFixed( );

		draw_vehicles( );

		// TODO(alex): to separate class `Updater`
		//# Update every road // self.roads[i].update(self.dtSpeeded, self.t)
		{
		for ( auto & road : m_roads ) {
			Sim::vehicles_t vehicles_ = road.getVehicles( );
			if ( vehicles_.empty( ) )
				continue;
			std::vector< Sim::IVehicle * > vehicles( 
				vehicles_.begin( ), vehicles_. end( ) );
			Sim::IVehicle* lead = vehicles.front( );

			//if ( false ) // tmp
			if ( road.traffic_signal_state( ) ) {
				//# If traffic signal is green (or doesnt exist), let vehicles pass
				lead ->unstop( t );
				for ( auto & vehicle : vehicles ) 
					vehicle ->unslow( );
			} else {
				if ( road.hasTrafficSignal( ) ) {
					//# The traffic signal is red (existence checked to access its stop_distance)
					bool lead_can_stop_safely = false;
					if ( lead ->x( ) <= ( road.length( ) - m_trafficSignal ->c_stopDistance / 1.5 ) )
						lead_can_stop_safely = true;
					if ( lead_can_stop_safely ) {
						//# slow vehicles in slow zone
						lead ->slow( m_trafficSignal ->c_slowFactor );
						bool lead_in_stop_zone = false;
						if ( ( road.length( ) - m_trafficSignal ->c_stopDistance ) <= lead ->x( ) )
							lead_in_stop_zone = true;
						if ( lead_in_stop_zone )
							lead ->stop( t );
					}
				}
			}

			//# Update first vehicle
			lead ->update( nullptr, dtSpeeded, &road );
			//# Update other vehicles
			for ( uint i = 1; i < vehicles.size( ); ++i ) { 
				lead = vehicles[ i - 1 ];
				vehicles[ i ] ->update( lead, dtSpeeded, &road );
			}
		}
		}

		// TODO(alex): to separate class `Updater`
		// vehicleGenerator update
		//if ( false ) // tmp
		{
		auto road_index = m_vehicleGenerator ->update( t, &n_vehicles_generated );
		if ( road_index ) {
			++n_vehicles_generated;
			++n_vehicles_on_map;
			// TODO(alex): makeme
			//self._non_empty_roads.add(road_index)
		}
		}

		// TODO(alex): to separate class `Updater`
		// self._check_out_of_bounds_vehicles()
		for ( auto & road : m_roads ) {
			Sim::vehicles_t const& vehicles = road.getVehicles( );
			if ( vehicles.empty( ) ) 
				continue;
			Sim::IVehicle* lead = vehicles[ 0 ];
			//# If first vehicle is out of road bounds
			if ( lead ->x( ) < road.length( ) ) 
				continue;
			auto &path = lead ->path( );

			// +TODO(alex): Throught all road segments
			{
				Sim::Road * currentRoad = nullptr;
				qreal carPosition = lead ->x( );
				uint followingIdxRoadIndex = lead ->currentIdxRoadIndex( );
				do {
					currentRoad = &m_roads[ path[ followingIdxRoadIndex ] ];
					qreal currentRoadLength = currentRoad ->length( );
					if ( ( carPosition - currentRoadLength ) <= 0 )
						break;
					carPosition -= currentRoadLength;
					++followingIdxRoadIndex;
					if ( followingIdxRoadIndex >= path.size( ) )
						break;
				} while( true );
				//# If vehicle hasnt a next road
				if ( followingIdxRoadIndex >= path.size( ) ) {
					//# Remove it from its road
					road.popFrontVehicle( );
					// TODO(alex): makeme //# Remove from non_empty_roads if it has no vehicles //if not road.vehicles: //new_empty_roads.add(road.index)
					--n_vehicles_on_map;
					//# Update the waiting times sum //self._waiting_times_sum += lead.get_wait_time(self.t)
					continue;
				}
				// Remove it from its road
				road.popFrontVehicle( );
				// Reset the position relative to the road
				lead ->setPositionOnRoad( carPosition );
				// Add it to the next road
				lead ->setIdxRoadIndex( followingIdxRoadIndex );
				Sim::Road * inCarRoad = nullptr;
				inCarRoad = &m_roads[ path[ followingIdxRoadIndex ] ];
				inCarRoad ->addVehicle( lead );
				continue;
			}
		}

		// TODO(alex): to separate class `Updater`
		// update signals
		{
#pragma region clear previous
		{
			auto it = std::remove_if( m_trafficSignalItems.begin( ), m_trafficSignalItems.end( )
					, [this](QGraphicsPolygonItem *p) {
						return scene( ) ->removeItem( p ), delete p, true;
					}
				);
			m_trafficSignalItems.erase( it, m_trafficSignalItems.end( ) );
		}
#pragma endregion
		Sim::Road::TrafficSignal::signalRoads_t signalRoads = 
			m_trafficSignal ->getRoads( );
		for ( uint i = 0; i < signalRoads.size( ); ++i ) {
			QColor color;
			QColor red = QColor::fromRgb( 255, 0, 0 );
			QColor green = QColor::fromRgb( 0, 255, 0 );
			auto currentCycle = m_trafficSignal ->getCurrentCycle( );
			const std::array<bool, 2> stat{ false, false };
			bool b = ( currentCycle == stat );
			if ( b ) {
				QColor yellow = QColor::fromRgb( 255, 255, 0 );
				uint currentCycleIndex = m_trafficSignal ->currentCycleIndex( );
				auto cycle = m_trafficSignal ->cycle( );
				if ( cycle[ currentCycleIndex - 1 ][ i ] )
					color = yellow;
				else 
					color = red;
			} else {
				if ( currentCycle[ i ] )
					color = green;
				else
					color = red;
			}
            for ( auto &road : m_trafficSignal ->getRoads( )[i] ) {
                qreal a = 0;
                QPointF position {
						(1 - a) * road ->end( ).x( ) + a * road ->start( ).x( )
						, (1 - a) * road ->end( ).y( ) + a * road ->start( ).y( )
					};
				auto points = m_draw ->rotated_box( position, { 1, 3 },
                                    road ->angle_cos( ), road ->angle_sin( ), true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				QPen pen( color );
				QBrush brush( color, Qt::SolidPattern );
				auto trafficSignalItem = scene( ) ->addPolygon( polygon, pen, brush );
				m_trafficSignalItems.push_back( trafficSignalItem );
			}
		}
		}

		// TODO(alex): to separate class `Updater`
		// _update_signals
		if ( c_dtZero == m_dtSwitchLast ) {
			m_dtSwitchLast = now( );
		} else {
			if ( m_dtLast > ( m_dtSwitchLast + m_timeToReswitch ) ) {
				m_dtSwitchLast = now( );
				m_trafficSignal ->update( );
			}
		}

		// TODO(alex): to separate class `Timing`
        //# Increment time
//		m_t += dt;
//		m_timing.incrementTime( );
	}

public: 
    MainQGraphicsView(QMainWindow *parent) : 
		DraggableQGraphicsView( parent )
	{
		m_dtLast = now( );
		// @insp https://stackoverflow.com/questions/28728820/qgraphicsview-doesnt-always-update
		QObject::connect( &m_timer, &QTimer::timeout
				, [this] { 
					this ->animation( );
				}
			);
		m_timer.start( 50 );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
