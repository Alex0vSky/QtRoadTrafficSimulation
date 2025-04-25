#pragma once // src\Updater.h - update all model entities
namespace syscross::TraffModel {
/**
 * Core simulation manager that handles:
 * 1. Vehicle movement physics on roads
 * 2. Traffic light state updates
 * 3. Vehicle cleanup at road boundaries
 *
 * Uses discrete time steps (dt) for synchronization.
 */
class Updater {
	using Road = Sim::Road;

    /// Collection of all road segments in simulation
	Road::roads_t *m_roads;

	/// Central traffic light controller
	Road::TrafficSignal *m_trafficSignal;

	/// Vehicle generation system
	Sim::VehicleGenerator *m_vehicleGenerator;

	/// Fixed interval between traffic light changes (seconds)
	Timing::timer_t m_magicTimeToSwitch = 25;

	/// Next scheduled traffic light switch time
	Timing::timer_t m_nextSwitch = 0;

public:
	/**
	 * @brief Constructs the simulation updater
	 * @param roads Container of all road segments
	 * @param trafficSignal Traffic light controller instance
	 * @param vehicleGenerator Vehicle spawn system reference
	 */
	Updater(Road::roads_t *r, Road::TrafficSignal *s, Sim::VehicleGenerator *g) :
		m_roads( r )
		, m_trafficSignal( s )
		, m_vehicleGenerator( g )
	{}

	/**
	 * @brief Updates vehicle dynamics and interactions
	 *
	 * Processes complete vehicle simulation step:
	 * - Acceleration/braking physics
	 * - Traffic signal responses
	 * - Inter-vehicle collision avoidance
	 *
	 * @param t Current simulation time (seconds)
	 * @param dt Time delta since last update (seconds)
	 */
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

	/**
	 * @brief Processes vehicles at road boundaries
	 *
	 * Handles vehicles exiting road segments:
	 * - Transfers between connected roads
	 * - Removes completed routes
	 * - Updates active vehicle count
	 *
	 * @param pVehiclesOnMap Pointer to active vehicle counter
	 */
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
			//# Remove it from its road
			Sim::IVehicle *front = road.popFrontVehicle( );
			//# If vehicle hasnt a next road
			if ( followingIdxRoadIndex >= path.size( ) ) {
				m_vehicleGenerator ->removeVehicle( front );
				--(*pVehiclesOnMap);
				//# Update the waiting times sum //self._waiting_times_sum += lead.get_wait_time(self.t)
				continue;
			}
			// Reset the position relative to the road
			lead ->setPositionOnRoad( carPosition );
			// Add it to the next road
			lead ->setIdxRoadIndex( followingIdxRoadIndex );
			Sim::Road * inCarRoad = &(*m_roads)[ path[ followingIdxRoadIndex ] ];
			inCarRoad ->addVehicle( lead );
		}
	}

	/**
	 * @brief Updates traffic signal states and timing
	 *
	 * Manages traffic light cycle:
	 * - Fixed-time intervals between phase changes
	 * - No sensor input (time-based only)
	 * - Handles green/yellow/red transitions
	 *
	 * @param t Current simulation time in seconds
	 */
	void trafficSignals(Timing::timer_t t) {
		if ( qFuzzyIsNull( m_nextSwitch ) )
			return m_nextSwitch = t + m_magicTimeToSwitch, (void)0;
		// insurance
		constexpr qreal epsilon = 1e-6;
		if ( t < m_nextSwitch - epsilon )
			return;
		m_nextSwitch = t + m_magicTimeToSwitch;
		m_trafficSignal ->update( t );
	}
};
} // namespace syscross::TraffModel
