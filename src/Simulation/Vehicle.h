#pragma once // src\Simulation\Vehicle.h - vehicle
namespace syscross::TraffModel::Sim {
/**
 * Vehicle entity with movement physics
 */
class Vehicle : public IVehicle {
	/// Unique vehicle identifier
	uint m_index = 0;

	/// Current road segment index in path
	uint m_idxRoadIndex = 0;

	/// Maximum allowed speed (m/s)
	qreal m_v_max_;

	/// Current speed (m/s)
	qreal m_v;

	/// Current acceleration (m/s²)
	qreal m_a = 0;

	/// Position on current road (m)
	qreal m_x = 0;

	/// Stopped state flag
	bool m_isStopped = false;

	/// Removal marker
	bool m_isRemoved = false;

	/// Vehicle length (m)
	uint m_length = 4;

	/// Vehicle width (m)
	uint m_width = 2;

	/// Minimum following distance (m)
	uint m_distanceDetweenNeighbor = 4; // s0

	/// Reaction time (s)
	uint m_T = 1;

	/// Max speed (m/s)
	qreal m_v_max = 16.6;

	/// Max acceleration (m/s²)
	qreal m_a_max = 1.44;

	/// Max braking (m/s²)
	qreal m_b_max = 4.61;

	/// Precomputed sqrt(a_max * b_max)
	qreal m_sqrt_ab;

	/// Last stop timestamp
	qreal m_lastTimeStopped = 0;

	/// Total waiting time (s)
	qreal m_waitingTime = 0;

	/// Planned route, road indexes
	std::vector< uint > m_path;

public:
	/**
	 * @brief Constructs vehicle with optional path
	 * @param path Route sequence of road indexes
	 */
	Vehicle(std::vector<uint> path = { } ) :
		m_path ( path )
	{
		m_sqrt_ab = 2 * std::sqrt( m_a_max * m_b_max );
		m_v_max_ = m_v_max;
	// OR create moving object
		m_v = m_v_max;
		// OR create stoping object
		//m_v = 0;
	}

	/**
	 * @brief Gets vehicle position on current road
	 * @return Position in meters from road start
	 */
	qreal x() const override {
		return m_x;
	}

	/**
	 * @brief Gets current speed
	 * @return Speed in meters/second
	 */
	qreal v() const override {
		return m_v;
	}

	/**
	 * @brief Gets vehicle length
	 * @return Length in meters
	 */
	uint length() const override {
		return m_length;
	}

	/**
	 * @brief Gets vehicle width
	 * @return Width in meters
	 */
	uint width() const override {
		return m_width;
	}

	/**
	 * @brief Gets planned route
	 * @return Const reference to road index sequence
	 */
	std::vector<uint> const& path() const override {
		return m_path;
	}

	/**
	 * @brief Gets current road index in path
	 * @return Index in path sequence
	 */
	uint currentIdxRoadIndex() const override {
		return m_idxRoadIndex;
	}

	/**
	 * @brief Resets position to road start
	 */
	void resetPositionOnRoad() override {
		m_x = 0;
	}

	/**
	 * @brief Advances to next road in path
	 */
	void incCurrentIdxRoadIndex() override {
		++m_idxRoadIndex;
	}

	/**
	 * @brief Assigns unique vehicle ID
	 * @param index Identifier to assign
	 */
	void setVehicleIndex(uint index) override {
		m_index = index;
	}

	/**
	 * @brief Sets position on current road
	 * @param x Position in meters from road start
	 */
	void setPositionOnRoad(qreal x) override {
		m_x = x;
	}

	/**
	 * @brief Changes current road in path
	 * @param idxRoadIndex New road sequence index
	 */
	void setIdxRoadIndex(uint idxRoadIndex) override {
		m_idxRoadIndex = idxRoadIndex;
	}

	/**
	 * @brief Updates vehicle physics state
	 *
	 * Calculates:
	 * - New position via kinematic equations
	 * - Acceleration based on:
	 *   - Leading vehicle
	 *   - Traffic signals
	 *   - Speed limits
	 *
	 * @param lead Preceding vehicle or nullptr
	 * @param dt Time step (s)
	 * @param road Current road segment
	 */
	void update(IVehicle* lead, qreal dt, Road* road) override {
		//# Update position and velocity
		if ( ( m_v + m_a * dt ) < 0) {
			m_x -= 1 / 2 * m_v * m_v / m_a;
			m_v = 0;
		} else {
			m_v += m_a * dt;
			m_x += m_v * dt + m_a * dt * dt / 2;
		}
		//# Update acceleration
		qreal alpha = 0;
		if ( lead ) {
			const qreal delta_x = lead->x() - m_x - lead->length();
			if ( delta_x <= 0.0 ) {
				alpha = std::numeric_limits<qreal>::max( );
			} else {
				const qreal delta_v = m_v - lead->v( );
				alpha = (m_distanceDetweenNeighbor + std::max(0.0, m_T * m_v + delta_v * m_v / m_sqrt_ab)) / delta_x;
			}
		}
		m_a = m_a_max * (1 - std::pow(m_v / m_v_max, 4) - std::pow(alpha, 2));
		if ( m_isStopped ) {
			m_a = -m_b_max * m_v / m_v_max;
		}
	}

	/**
	 * @brief Stops vehicle movement
	 * @param t Current simulation time when stopping occurs
	 */
	void stop(qreal t) override {
		if ( !m_isStopped ) {
			m_lastTimeStopped = t;
			m_isStopped = true;
		}
	}

	/**
	 * @brief Resumes vehicle movement
	 * @param t Current simulation time when resuming
	 */
	void unstop(qreal t) override {
		if ( m_isStopped ) {
			m_waitingTime += (t - m_lastTimeStopped);
			m_lastTimeStopped = NULL;
			m_isStopped = false;
		}
	}

	/**
	 * @brief Applies speed reduction factor
	 * @param traffic_light_slow_factor Speed multiplier (0.0-1.0)
	 */
	void slow(qreal traffic_light_slow_factor) override {
		m_v_max = m_v_max_ * traffic_light_slow_factor;
	}

	/**
	 * @brief Restores normal speed limits
	 */
	void unslow() override {
		m_v_max = m_v_max_;
	}

	/**
	 * @brief Gets minimum safe following distance
	 * @return Distance in meters maintained from vehicles ahead
	 */
	uint getDistanceDetweenNeighbor() const {
		return m_distanceDetweenNeighbor;
	}

	/**
	 * @brief Immediately sets vehicle speed to zero
	 * Emergency braking without physics simulation
	 */
	void zeroedSpeed() {
		m_v = 0;
	}

	/**
	 * @brief Gets assigned vehicle ID
	 * @return Current identifier
	 */
	uint getVehicleIndex() const override {
		return m_index;
	}
};
} // namespace syscross::TraffModel::Sim
