// src\Updater.h - update all model entities
namespace syscross::TraffModel {
class Updater { 
	using Road = Sim::Road;
	Road::roads_t *m_roads;
	Road::TrafficSignal *m_trafficSignal;
	// TODO(alex): magic until `Timing::timer_t` is floatPointNumber
	Timing::timer_t m_magicTimeToSwitch = 25;
	Timing::timer_t m_nextSwitch = 0;

public:
	Updater(Road::roads_t *roads, Road::TrafficSignal *trafficSignal) : 
		m_roads( roads )
		, m_trafficSignal( trafficSignal ) 
	{}
	//# Update every road
	void roads(Timing::timer_t t, Timing::timer_t dt) { 
		for ( auto & road : (*m_roads) ) {
			Sim::IVehicle::vehicles_t vehicles_ = road.getVehicles( );
			if ( vehicles_.empty( ) )
				continue;
			std::vector< Sim::IVehicle * > vehicles( 
				vehicles_.begin( ), vehicles_. end( ) );
			Sim::IVehicle* lead = vehicles.front( );

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
			lead ->update( nullptr, dt, &road );
			//# Update other vehicles
			for ( uint i = 1; i < vehicles.size( ); ++i ) { 
				lead = vehicles[ i - 1 ];
				vehicles[ i ] ->update( lead, dt, &road );
			}
		}
	}
	// _check_out_of_bounds_vehicles
	void outOfBoundsVehicles(uint *pVehiclesOnMap) { 
		for ( auto & road : (*m_roads) ) {
			Sim::IVehicle::vehicles_t const& vehicles = road.getVehicles( );
			if ( vehicles.empty( ) ) 
				continue;
			Sim::IVehicle* lead = vehicles[ 0 ];
			//# If first vehicle is out of road bounds
			if ( lead ->x( ) < road.length( ) ) 
				continue;
			auto &path = lead ->path( );

			Sim::Road * currentRoad = nullptr;
			qreal carPosition = lead ->x( );
			uint followingIdxRoadIndex = lead ->currentIdxRoadIndex( );
			do {
				currentRoad = &(*m_roads)[ path[ followingIdxRoadIndex ] ];
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
				--(*pVehiclesOnMap);
				//# Update the waiting times sum //self._waiting_times_sum += lead.get_wait_time(self.t)
				continue;
			}
			// Remove it from its road
			road.popFrontVehicle( );
			// Reset the position relative to the road
			lead ->setPositionOnRoad( carPosition );
			// Add it to the next road
			lead ->setIdxRoadIndex( followingIdxRoadIndex );
			Sim::Road * inCarRoad = &(*m_roads)[ path[ followingIdxRoadIndex ] ];
			inCarRoad ->addVehicle( lead );
		}
	}
	// _update_signals
	void trafficSignals(Timing::timer_t t) { 
		if ( !m_nextSwitch )
			return m_nextSwitch = t + m_magicTimeToSwitch, (void)0;
		if ( t < m_nextSwitch )
			return;
		m_nextSwitch = t + m_magicTimeToSwitch;
		m_trafficSignal ->update( t );
	}
};
} // namespace syscross::TraffModel
