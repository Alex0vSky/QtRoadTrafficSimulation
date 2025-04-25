#pragma once // src\Simulation\Vehicle.h - vehicle
namespace syscross::TraffModel::Sim {
class Vehicle : public IVehicle {
    uint m_index = 0;
    uint m_idxRoadIndex = 0;
    qreal m_v_max_, m_v;
    qreal m_a = 0, m_x = 0;
    bool m_isStopped = false;
    bool m_isRemoved = false;
    uint m_length = 4;
    uint m_width = 2;
    uint m_distanceDetweenNeighbor = 4; // s0
    uint m_T = 1;
    qreal m_v_max = 16.6, m_a_max = 1.44, m_b_max = 4.61;
    qreal m_sqrt_ab;
    qreal m_lastTimeStopped = 0, m_waitingTime = 0;
    std::vector< uint > m_path;
    //std::pair< double, qreal > m_position;

public:
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
    qreal x() const override {
		return m_x;
	}
    qreal v() const override {
		return m_v;
	}
    uint length() const override {
		return m_length;
	}
    uint width() const override {
		return m_width;
	}
	std::vector<uint> const& path() const override {
		return m_path;
	}
    uint currentIdxRoadIndex() const override {
		return m_idxRoadIndex;
	}
	void resetPositionOnRoad() override {
		m_x = 0;
	}
	void incCurrentIdxRoadIndex() override {
		++m_idxRoadIndex;
	}
    void setVehicleIndex(uint index) override {
		m_index = index;
	}
	void setPositionOnRoad(qreal x) override {
		m_x = x;
	}
	void setIdxRoadIndex(uint idxRoadIndex) override {
		m_idxRoadIndex = idxRoadIndex;
	}

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
        ////# Update position
        //qreal sin = road->angle_sin( );
        //qreal cos = road->angle_cos( );
        //qreal x = road->start( ).x( ) + cos * m_x;
        //qreal y = road->start( ).y( ) + sin * m_x;
        //m_position = std::make_pair(x, y);
    }
    void stop(qreal t) override {
        if ( !m_isStopped ) {
            m_lastTimeStopped = t;
            m_isStopped = true;
        }
    }
    void unstop(qreal t) override {
        if ( m_isStopped ) {
            m_waitingTime += (t - m_lastTimeStopped);
            m_lastTimeStopped = NULL;
            m_isStopped = false;
        }
    }
    void slow(qreal traffic_light_slow_factor) override {
        m_v_max = m_v_max_ * traffic_light_slow_factor;
	}
    void unslow() override {
        m_v_max = m_v_max_;
	}

	uint getDistanceDetweenNeighbor() const {
		return m_distanceDetweenNeighbor;
	}
	void zeroedSpeed() {
		m_v = 0;
	}

	uint getVehicleIndex() const override {
		return m_index;
	}
};
} // namespace syscross::TraffModel::Sim
