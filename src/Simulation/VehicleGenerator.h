#pragma once // src\Simulation\VehicleGenerator.h - rules of creating new vehicle on the road
#include "Simulation/AllRoads.h"
#include "Simulation/Vehicle.h"
namespace syscross::TraffModel::Sim {
/**
 * Manages vehicle spawning and route assignment
 */
class VehicleGenerator {
	/// Available routes with weights
	AllRoads::flatPathIndexes_t m_paths;

	/// Random number generator
	QRandomGenerator m_randomGenerator;

	/// Vehicles per minute spawn rate
	int m_vehicleRate;

	/// Last generation timestamp
	Timing::timer_t m_previousGenTime;

	/// Mapping of entry roads
	AllRoads::inboundRoads_t m_inboundRoads;

	/// Total vehicles generated
	uint m_generated = 0;

	typedef std::unique_ptr< Sim::Vehicle > vehicle_t;
	/// Vehicle storage
	std::unordered_map< Sim::IVehicle *, vehicle_t > m_vehiclesHolder;

public:
	/**
	 * @brief Constructs generator with spawn configuration
	 * @param vehicle_rate Vehicles per minute
	 * @param paths Available routes with weights
	 * @param inboundRoads Entry point roads
	 */
	VehicleGenerator(int vehicle_rate, AllRoads::flatPathIndexes_t paths, AllRoads::inboundRoads_t inboundRoads) :
		m_paths( paths )
		, m_randomGenerator( QRandomGenerator::securelySeeded( ) )
		, m_vehicleRate( vehicle_rate )
		, m_previousGenTime( 0 )
		, m_inboundRoads( inboundRoads )
	{}

	/**
	 * @brief Generates new vehicle with random route
	 *
	 * Selects route based on weight distribution:
	 * - Straight paths have 3x weight
	 * - Turns have 1x weight
	 *
	 * @return New vehicle instance
	 */
	vehicle_t generateVehicle() {
		AllRoads::weight_t total = 0;
		for ( auto & elem : m_paths )
			total += elem.first;
		auto r = m_randomGenerator.bounded( total + 1 );
		AllRoads::flatElem_t randElem = m_paths.front( );
		for ( auto & elem : m_paths ) {
            r -= elem.first;
            if ( r <= 0 ) {
				randElem = elem;
				break;
			}
		}
		return std::make_unique< Vehicle >( randElem.second );
	}

	/**
	 * @brief Attempts to spawn vehicle at appropriate time
	 *
	 * Handles:
	 * - Spawn timing checks
	 * - Road capacity validation
	 * - Vehicle storage management
	 *
	 * @param curr_t Current simulation time
	 * @return Road index if spawned, empty optional otherwise
	 */
	std::optional< uint > update(Timing::timer_t curr_t) {
		bool time_elapsed = curr_t - m_previousGenTime >= Timing::timer_t{ 60 } / m_vehicleRate;
		//# If there's no vehicles on the map, or if the time elapsed after last
		//# generation is greater than the vehicle rate, generate a vehicle
		if ( m_generated && !time_elapsed )
			return { };
		vehicle_t vehicle = generateVehicle( );
		uint firstRoadIdx = vehicle ->path( )[ 0 ];
		Road *road = m_inboundRoads[ firstRoadIdx ];
		IVehicle::vehicles_t roadVehicles = road ->getVehicles( );
		//# If the road is empty, or there's sufficient space for the generated vehicle, add it
		qreal freeSpace = 0;
		if ( !roadVehicles.empty( ) ) {
			uint needSpace = ( vehicle ->getDistanceDetweenNeighbor( ) + vehicle ->length( ) );
			qreal pos = roadVehicles.back( ) ->x( );
			freeSpace = pos - needSpace;
			if ( freeSpace <= 0 )
				return { };
			if ( freeSpace < 0.5 )
				vehicle ->zeroedSpeed( );
		}
		vehicle ->setVehicleIndex( m_generated );
		auto [it, inserted] = m_vehiclesHolder.try_emplace( vehicle.get( ), std::move( vehicle ) );
		if ( inserted )
			road ->addVehicle( it ->first );

		m_previousGenTime = curr_t;
		++m_generated;
		return road ->getIndex( );
	}

	/**
	 * @brief Removes vehicle from tracking
	 * @param p Vehicle to remove
	 */
	void removeVehicle(IVehicle *p) {
		if ( m_vehiclesHolder.end( ) == m_vehiclesHolder.find( p ) )
			return;
		m_vehiclesHolder.erase( p );
	}
};
} // namespace syscross::TraffModel::Sim
