// src\Simulation\Vehicle.h - vehicle
namespace syscross::TraffModel::Sim {
class Vehicle : public IVehicle {
    uint m_index = 0;
    uint m_idxRoadIndex = 0;
    qreal m_v_max_, m_v, m_a = 0, m_x = 0;
    bool m_isStopped = false;
    bool m_isRemoved = false;
    uint m_length = 4;
    uint m_width = 2;
    uint m_space = 4; // s0
    uint m_T = 1;
    qreal m_v_max = 16.6, m_a_max = 1.44, m_b_max = 4.61;
    qreal m_sqrt_ab;
    qreal m_lastTimeStopped = 0, m_waitingTime = 0;
    std::vector<uint> m_path;
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
        if ( ( this->m_v + this->m_a * dt ) < 0) {
            this->m_x -= 1 / 2 * this->m_v * this->m_v / this->m_a;
            this->m_v = 0;
        } else {
            this->m_v += this->m_a * dt;
            this->m_x += this->m_v * dt + this->m_a * dt * dt / 2;
        }
        //# Update acceleration
        qreal alpha = 0;
        if (lead) {
            qreal delta_x = lead->x( ) - this->m_x - lead->length( );
            qreal delta_v = this->m_v - lead->v( );
            alpha = (this->m_space + std::max(0.0, this->m_T * this->m_v + delta_v * this->m_v / this->m_sqrt_ab)) / delta_x;
        }
        this->m_a = this->m_a_max * (1 - std::pow(this->m_v / this->m_v_max, 4) - std::pow(alpha, 2));
        if (this->m_isStopped) {
            this->m_a = -this->m_b_max * this->m_v / this->m_v_max;
        }
        ////# Update position
        //qreal sin = road->angle_sin( );
        //qreal cos = road->angle_cos( );
        //qreal x = road->start( ).x( ) + cos * this->m_x;
        //qreal y = road->start( ).y( ) + sin * this->m_x;
        //this->m_position = std::make_pair(x, y);
    }
    void stop(qreal t) override {
        if (!this->m_isStopped) {
            this->m_lastTimeStopped = t;
            this->m_isStopped = true;
        }
    }
    void unstop(qreal t) override {
        if (this->m_isStopped) {
            this->m_waitingTime += (t - this->m_lastTimeStopped);
            this->m_lastTimeStopped = NULL;
            this->m_isStopped = false;
        }
    }
    void slow(qreal traffic_light_slow_factor) override {
        m_v_max = m_v_max_ * traffic_light_slow_factor;
	}
    void unslow() override {
        m_v_max = m_v_max_;
	}

	uint getSpace() const {
		return m_space;
	}

	uint getVehicleIndex() const override {
		return m_index;
	}
	void setRemoved() override {
		m_isRemoved = true;
	}
	bool isRemoved() const override {
		return m_isRemoved;
	}
};
} // namespace syscross::TraffModel::Sim
