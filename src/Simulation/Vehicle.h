// src\Simulation\Vehicle.h - vehicle
namespace Simulation { class Vehicle : public IVehicle {
    uint m_index = 0;
    double m_v_max_, m_v, m_a = 0, m_x = 0;
    bool m_isStopped = false;
    bool m_isRemoved = false;
    uint m_currentRoadIndex = 0;
    uint m_length = 4;
    uint m_width = 2;
    uint m_space = 4; // s0
    uint m_T = 1;
    double m_v_max = 16.6, m_a_max = 1.44, m_b_max = 4.61;
    double m_sqrt_ab;
    double m_lastTimeStopped = 0, m_waitingTime = 0;
    std::vector<uint> m_path;
    //std::pair< double, double > m_position;

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
    double x() const override {
		return m_x;
	}
    double v() const override {
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
    uint currentRoadIndex() const override {
		return m_currentRoadIndex;
	}
	void resetPositionOnRoad() override {
		m_x = 0;
	}
	void incCurrentRoadIndex() override {
		++m_currentRoadIndex;
	}
    void setVehicleIndex(uint index) override {
		m_index = index;
	}

    void update(IVehicle* lead, double dt, Road* road) override {
        //# Update position and velocity
        if ( ( this->m_v + this->m_a * dt ) < 0) {
            this->m_x -= 1 / 2 * this->m_v * this->m_v / this->m_a;
            this->m_v = 0;
        } else {
            this->m_v += this->m_a * dt;
            this->m_x += this->m_v * dt + this->m_a * dt * dt / 2;
        }
        //# Update acceleration
        double alpha = 0;
        if (lead) {
            double delta_x = lead->x( ) - this->m_x - lead->length( );
            double delta_v = this->m_v - lead->v( );
            alpha = (this->m_space + std::max(0.0, this->m_T * this->m_v + delta_v * this->m_v / this->m_sqrt_ab)) / delta_x;
        }
        this->m_a = this->m_a_max * (1 - std::pow(this->m_v / this->m_v_max, 4) - std::pow(alpha, 2));
        if (this->m_isStopped) {
            this->m_a = -this->m_b_max * this->m_v / this->m_v_max;
        }
        //# Update position
        double sin = road->angle_sin( );
        double cos = road->angle_cos( );
        double x = road->start( ).x( ) + cos * this->m_x;
        double y = road->start( ).y( ) + sin * this->m_x;
        //this->m_position = std::make_pair(x, y);
    }
    void stop(double t) override {
        if (!this->m_isStopped) {
            this->m_lastTimeStopped = t;
            this->m_isStopped = true;
        }
    }
    void unstop(double t) override {
        if (this->m_isStopped) {
            this->m_waitingTime += (t - this->m_lastTimeStopped);
            this->m_lastTimeStopped = NULL;
            this->m_isStopped = false;
        }
    }
    void slow(double traffic_light_slow_factor) override {
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
} // namespace Simulation 
