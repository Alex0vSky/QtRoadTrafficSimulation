// src\Simulation\VehicleGenerator.h - rules of creating new vehicle on the road
namespace syscross::TraffModel::Sim {
class VehicleGenerator {
	AllRoads::flatPathIndexes_t m_paths;
	QRandomGenerator m_randomGenerator;
    int m_vehicleRate;
    Timing::timer_t m_previousGenTime;
	AllRoads::inboundRoads_t m_inboundRoads;
	uint m_generated = 0;
	// TODO(alex): refactorme to using `IVehicle::isRemoved( )`
	static const uint c_maxVehicles = 4096;
	// fixed memory for pointers
	std::array< Sim::Vehicle, c_maxVehicles > m_generatedVehicles;

public:
	VehicleGenerator(int vehicle_rate, AllRoads::flatPathIndexes_t paths, AllRoads::inboundRoads_t inboundRoads) :
		m_paths( paths )
		, m_randomGenerator( QRandomGenerator::securelySeeded( ) )
        , m_vehicleRate( vehicle_rate )
        , m_previousGenTime( 0 )
        , m_inboundRoads( inboundRoads )
	{}
	Vehicle generateVehicle() {
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
		return Vehicle( randElem.second );
	}
	std::optional< uint > update(Timing::timer_t curr_t) {
		bool time_elapsed = curr_t - m_previousGenTime >= Timing::timer_t{ 60 } / m_vehicleRate;
		//# If there's no vehicles on the map, or if the time elapsed after last
		//# generation is greater than the vehicle rate, generate a vehicle
		if ( !m_generated || time_elapsed ) {
			Vehicle vehicle = generateVehicle( );
			uint firstRoadIdx = vehicle.path( )[ 0 ];
			Road *road = m_inboundRoads[ firstRoadIdx ];
			auto roadVehicles = road ->getVehicles( );
			bool isEmpty = roadVehicles.empty( );
			//# If the road is empty, or there's sufficient space for the generated vehicle, add it
			if ( isEmpty || roadVehicles.back( ) ->x( ) > ( vehicle.getSpace( ) + vehicle.length( ) ) ) {
				vehicle.setVehicleIndex( m_generated );
				uint nextVehicle = m_generated;
				//m_generatedVehicles[ 0 ].isRemoved( );
				m_generatedVehicles[ nextVehicle ] = vehicle;
				road ->addVehicle( &m_generatedVehicles[ nextVehicle ] );
				m_previousGenTime = curr_t;
				++m_generated;
				return road ->getIndex( );
			}
		}
		return { };
	}
};
} // namespace syscross::TraffModel::Sim
