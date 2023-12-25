// src\Simulation\VehicleGenerator.h - rules of creating new vehicle on the road
namespace Simulation {
class VehicleGenerator {
	AllRoads::flatPathIndexes_t m_paths;
	QRandomGenerator m_randomGenerator;
    int m_vehicle_rate;
    timer_t m_prev_gen_time;
	AllRoads::inboundRoads_t m_inboundRoads;
	// TODO(alex): refactorme to using `IVehicle::isRemoved( )`
	static const uint c_maxVehicles = 4096;
	// fixed memory for pointers
	std::array< Simulation::Vehicle, c_maxVehicles > m_generatedVehicles;

public:
	VehicleGenerator(int vehicle_rate, AllRoads::flatPathIndexes_t paths, AllRoads::inboundRoads_t inboundRoads) :
		m_paths( paths )
		, m_randomGenerator( QRandomGenerator::securelySeeded( ) )
        , m_vehicle_rate( vehicle_rate )
        , m_prev_gen_time( 0 )
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
	std::optional< uint > update(timer_t curr_t, uint *n_vehicles_generated) {
		//bool time_elapsed = (curr_t - this->_prev_gen_time) >= (60 / this->_vehicle_rate);

		bool time_elapsed = curr_t - m_prev_gen_time >= timer_t{ 60 } / m_vehicle_rate;
		//# If there's no vehicles on the map, or if the time elapsed after last
		//# generation is greater than the vehicle rate, generate a vehicle
		if ( !*n_vehicles_generated || time_elapsed ) {
			Vehicle vehicle = generateVehicle( );
			uint firstRoadIdx = vehicle.path( )[ 0 ];
			Road *road = m_inboundRoads[ firstRoadIdx ];
			auto roadVehicles = road ->getVehicles( );
			bool isEmpty = roadVehicles.empty( );
			//# If the road is empty, or there's sufficient space for the generated vehicle, add it
			if ( isEmpty || roadVehicles.back( ) ->x( ) > ( vehicle.getSpace( ) + vehicle.length( ) ) ) {
				if ( !isEmpty ) {
					roadVehicles.back( ) ->x( );
					vehicle.getSpace( ) + vehicle.length( );
				}
				vehicle.setVehicleIndex( *n_vehicles_generated );
				uint nextVehicle = *n_vehicles_generated;
				//m_generatedVehicles[ 0 ].isRemoved( );
				m_generatedVehicles[ nextVehicle ] = vehicle;
				road ->addVehicle( &m_generatedVehicles[ nextVehicle ] );
				m_prev_gen_time = curr_t;
				return road ->getIndex( );
			}
		}
		return { };
	}
};
} // namespace Simulation 
