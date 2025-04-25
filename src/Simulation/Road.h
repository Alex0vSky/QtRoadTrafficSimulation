#pragma once // src\Simulation\Road.h - indeed
#include "Timing.h"
#include "Simulation/IVehicle.h"
namespace syscross::TraffModel::Sim {
/**
 * Road segment with vehicle management
 */
class Road {
	/// Contained vehicles in traversal order
	IVehicle::vehicles_t m_cars;

	/// Unique road identifier
	uint m_index = 0;

	/// Segment length in meters
	qreal m_distance;

	/// Start point coordinates
	QPointF m_start;

	/// End point coordinates
	QPointF m_end;

	/// Sine of road angle
	qreal m_angle_sin;

	/// Cosine of road angle
	qreal m_angle_cos;

	friend class AllRoads;
	void setIndex(uint index) {
		m_index = index;
	}

public:
	typedef std::vector< Road > roads_t;
	/**
	 * Traffic light controller for road network
	 */
	class TrafficSignal {
		/// Signal phase cycle [group][phase]
		std::vector< std::array<bool, 2> > m_cycle {
				{ false, true }
				, { false, false }
				, { true, false }
				, { false, false }
			};
		/// Current phase index
		uint m_currentCycleIndex = 0;
	public:
		typedef std::vector< std::vector< Road *> > signalRoads_t;
	private:
		/// Signal-controlled road pairs
		signalRoads_t m_signalRoads;
	public:
		/// Minimum braking distance in meters
		double const c_slowDistance = 50;
		/// Speed reduction factor
		double const c_slowFactor = 0.4;
		/// Full stop distance in meters
		double const c_stopDistance = 15;
		// TODO(alex): semantics this consts isnt about traffic signal

		/**
		 * @brief Constructs controller with road configuration
		 * @param signalRoads Road pairs sharing signals
		 */
		TrafficSignal(signalRoads_t const& signalRoads) :
			m_signalRoads( signalRoads )
		{
			for ( uint i = 0; i < signalRoads.size( ); ++i )
				for ( auto road : signalRoads[ i ] )
					road ->setTrafficSignal( this, i );
		}

		/**
		 * @brief Advances signal phase
		 *
		 * Handles:
		 * - Cycle progression
		 * - Phase timing
		 * - State transitions
		 *
		 * @param t Current simulation time
		 */
		void update(Timing::timer_t) {
			m_currentCycleIndex = ( m_currentCycleIndex + 1 ) % m_cycle.size( );
		}

		/**
		 * @brief Gets current phase states
		 * @return Array of active signals per group
		 */
		std::array<bool, 2> getCurrentCycle() const {
			return m_cycle[ m_currentCycleIndex ];
		}

		/**
		 * @brief Gets controlled roads
		 * @return Road pairs by signal group
		 */
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
	/// Traffic signal association flag
	bool m_hasTrafficSignal = false;

	/// Traffic controller reference
	TrafficSignal *m_trafficSignal = nullptr;

	/// Signal control group index
	uint m_trafficSignalGroup = 0;
	void setTrafficSignal(TrafficSignal *trafficSignal, uint group) {
        m_hasTrafficSignal = true;
        m_trafficSignal = trafficSignal;
        m_trafficSignalGroup = group;
	}

public:
	/**
	 * @brief Checks traffic signal state
	 *
	 * Evaluates:
	 * - Current signal cycle phase
	 * - Group assignment
	 *
	 * @return True if allowed to pass
	 */
	bool traffic_signal_state() {
		if ( !m_hasTrafficSignal )
			return true;
		return m_trafficSignal ->getCurrentCycle( )[ m_trafficSignalGroup ];
	}

	bool hasTrafficSignal() const {
		return m_hasTrafficSignal;
	}

	/**
	 * @brief Constructs road segment
	 * @param start Begin coordinates
	 * @param end Finish coordinates
	 */
	Road(QPointF start, QPointF end) :
		m_start( start )
		, m_end( end )
		// @insp https://www.qtcentre.org/threads/51253-Why-doesn-t-QPointF-have-a-norm-function
		, m_distance( QLineF( start, end ).length( ) )
		, m_angle_sin( ( end.y( ) - start.y( ) ) / m_distance )
		, m_angle_cos( ( end.x( ) - start.x( ) ) / m_distance )
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
	uint getIndex() const {
		return m_index;
	}

	/**
	 * @brief Gets vehicles on road
	 * @return Vehicle collection
	 */
	IVehicle::vehicles_t getVehicles() {
		return m_cars;
	}

	/**
	 * @brief Adds vehicle to road start
	 * @param p Vehicle to add
	 */
	void addVehicle(IVehicle *p) {
//		qDebug( ) << "road" << m_index << "addVehicle" << p << p ->getVehicleIndex( );
		m_cars.push_back( p );
	}

	/**
	 * @brief Removes leading vehicle
	 * @return Removed vehicle
	 */
	IVehicle *popFrontVehicle() {
		IVehicle *p = m_cars.front( );
//		qDebug( ) << "road" << m_index << "popFrontVehicle" << p << p ->getVehicleIndex( );
		m_cars.erase( m_cars.begin( ) );
		return p;
	}
};
} // namespace syscross::TraffModel::Sim
