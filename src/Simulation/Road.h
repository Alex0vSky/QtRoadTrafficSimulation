// src\Simulation\Road.h - road segment
namespace Simulation {
class Road {
	vehicles_t m_vehicles;
	uint m_index;
	QPointF m_start, m_end;
	qreal m_distance;
	qreal m_angle_sin, m_angle_cos;
	friend class AllRoads;
	void setIndex(uint index) {
		m_index = index;
	}

public:
	typedef std::vector< Road > roads_t;

	class TrafficSignal {
		std::vector< std::array<bool, 2> > m_cycle { 
				{ false, true }
				, { false, false }
				, { true, false }
				, { false, false }
			};
		uint m_currentCycleIndex = 0;
	public:
		typedef std::vector< std::vector< Road *> > signalRoads_t;
	private:
		signalRoads_t m_signalRoads;
	public:
		double const c_slowDistance = 50, c_slowFactor = 0.4, c_stopDistance = 15;
		TrafficSignal(signalRoads_t const& signalRoads) :
			m_signalRoads( signalRoads )
		{
			for ( uint i = 0; i < signalRoads.size( ); ++i ) {
				for ( auto road : signalRoads[ i ] ) {
					road ->setTrafficSignal( this, i );
				}
			}
		}
		void update() {
			m_currentCycleIndex = ( m_currentCycleIndex + 1 ) % m_cycle.size( );
		}
		std::array<bool, 2> getCurrentCycle() const {
			return m_cycle[ m_currentCycleIndex ];
		}
		signalRoads_t const& getRoads() const {
			return m_signalRoads;
		}
		uint currentCycleIndex() const {
			return m_currentCycleIndex;
		}
		auto &cycle() const {
			return m_cycle;
		}
	};

private:
	bool m_hasTrafficSignal = false;
	TrafficSignal *m_trafficSignal;
	uint m_trafficSignalGroup = 0;
	void setTrafficSignal(TrafficSignal *trafficSignal, uint group) {
        m_hasTrafficSignal = true;
        m_trafficSignal = trafficSignal;
        m_trafficSignalGroup = group;
	}

public:
    bool traffic_signal_state() {
        if ( !m_hasTrafficSignal ) 
			return true;
		return m_trafficSignal ->getCurrentCycle( )[ m_trafficSignalGroup ];
    }
    bool hasTrafficSignal() const {
        return m_hasTrafficSignal;
    }
	Road(QPointF start, QPointF end) :
		m_start( start )
		, m_end( end )
		// @insp https://www.qtcentre.org/threads/51253-Why-doesn-t-QPointF-have-a-norm-function
		, m_distance( QLineF( m_start, m_end ).length( ) )
		, m_angle_sin( ( m_end.y( ) - m_start.y( ) ) / m_distance )
		, m_angle_cos( ( m_end.x( ) - m_start.x( ) ) / m_distance )
	{}
	qreal length() const {
		return m_distance;
	}
	qreal angle_sin() const { //self.angle_sin: float = (self.m_end[1] - self.m_start[1]) / self.length
		return m_angle_sin;
	}
	qreal angle_cos() const { //self.angle_cos: float = (self.m_end[0] - self.m_start[0]) / self.length
		return m_angle_cos;
	}
	QPointF start() const { 
		return m_start;
	}
	QPointF end() const { 
		return m_end;
	}

	void addVehicle(IVehicle *p) { 
		//qDebug( ) << "road" << m_index << "addVehicle" << p << p ->getVehicleIndex( );
		m_vehicles.push_back( p );
	}
	vehicles_t getVehicles() { 
		return m_vehicles;
	}
    //virtual void popFrontVehicle() = 0;
	//void popFrontVehicle() {
	void popFrontVehicle() {
		//auto p = m_vehicles.front( ); qDebug( ) << "road" << m_index << "popFrontVehicle" << p << p ->getVehicleIndex( );
		m_vehicles.erase( m_vehicles.begin( ) );
	}
	void removeFrontVehicle() {
		m_vehicles.front( ) ->setRemoved( );
		m_vehicles.erase( m_vehicles.begin( ) );
	}

	uint getIndex() const {
		return m_index;
	}

};
} // namespace Simulation 
