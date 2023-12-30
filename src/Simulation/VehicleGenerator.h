// src\Simulation\VehicleGenerator.h - rules of creating new vehicle on the road
namespace syscross::TraffModel::Sim {
class VehicleGenerator {
	AllRoads::flatPathIndexes_t m_paths;
	QRandomGenerator m_randomGenerator;
    int m_vehicleRate;
    Timing::timer_t m_previousGenTime;
	AllRoads::inboundRoads_t m_inboundRoads;
	uint m_generated = 0;
	typedef std::unique_ptr< Sim::Vehicle > vehicle_t;
	std::unordered_map< Sim::IVehicle *, vehicle_t > m_vehiclesHolder;

public:
	VehicleGenerator(int vehicle_rate, AllRoads::flatPathIndexes_t paths, AllRoads::inboundRoads_t inboundRoads) :
		m_paths( paths )
		, m_randomGenerator( QRandomGenerator::securelySeeded( ) )
        , m_vehicleRate( vehicle_rate )
        , m_previousGenTime( 0 )
        , m_inboundRoads( inboundRoads )
	{}
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
		auto pair = m_vehiclesHolder.emplace( vehicle.get( ), std::move( vehicle ) );
		road ->addVehicle( pair.first ->first );

		m_previousGenTime = curr_t;
		++m_generated;
		return road ->getIndex( );
	}
	void removeVehicle(IVehicle *p) {
		if ( m_vehiclesHolder.end( ) == m_vehiclesHolder.find( p ) )
			return;
		m_vehiclesHolder.erase( p );
	}
};
} // namespace syscross::TraffModel::Sim
