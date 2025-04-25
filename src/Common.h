#pragma once // src\Common.h - common members for child class
#include "Simulation/Road.h"
#include "Simulation/VehicleGenerator.h"
#include "Updater.h"
#include "Scener.h"
namespace syscross::TraffModel {
/**
 * Shared simulation components and state
 */
struct Common {
protected:
	/// Collection of all road segments
	Sim::Road::roads_t m_roads;

	/// Vehicle generation frequency (vehicles/minute)
	static const uint c_vehicleRate = 35;

	/// Vehicle generator instance
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;

	/// Traffic light controller
	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;

	/// Current active vehicle count
	uint m_vehiclesOnMap = 0;

	/// Time management utilities
	Timing m_timing;

	/// Simulation state updater
	std::unique_ptr< Updater > m_updater;

	/// Visualization handler
	std::unique_ptr< Scener > m_scener;

	/**
	 * @brief Initializes simulation components
	 *
	 * Sets up:
	 * - Road network
	 * - Vehicle generator
	 * - Traffic signals
	 * - Core systems
	 */
	void init() {
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
		//m_roads[ roadIndex ].addVehicle(vehicle.get( ) );

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

		m_updater = std::make_unique< Updater >( &m_roads, m_trafficSignal.get( ), m_vehicleGenerator.get( ) );
		m_scener = std::make_unique< Scener >( &m_roads, m_trafficSignal.get( ) );
	}
};
} // namespace syscross::TraffModel
