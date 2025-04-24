#pragma once // src\Timing.h - solve timing task
namespace syscross::TraffModel {
class Timing { 
	typedef std::chrono::steady_clock clock_t;

public:
	// Python time model was floatPointNumber
	typedef double timer_t;

private:
	static constexpr auto now = clock_t::now;
	typedef std::tuple< timer_t, timer_t > return_t;

	typedef std::chrono::milliseconds pythonModelTimeResolution_t;
	static constexpr auto c_xxxInSecond = std::chrono::duration_cast
		< pythonModelTimeResolution_t >
		( std::chrono::seconds{ 1 } ).count( );

	clock_t::time_point m_lastClockTimePoint = { };
	timer_t m_t = 0, m_lastDeltaTime = 0;
	qreal m_speed = 3; //24;

	void incrementTime_() { 
		m_t += m_lastDeltaTime;
	}
	void measure_() { 
		auto timePoint = now( );
		auto durationFrame = std::chrono::duration_cast
			< pythonModelTimeResolution_t >
			( timePoint - m_lastClockTimePoint );
		m_lastClockTimePoint = timePoint;
		m_lastDeltaTime = static_cast< timer_t >( durationFrame.count( ) ) / c_xxxInSecond;
		m_lastDeltaTime *= m_speed;
		m_t += m_lastDeltaTime;
	}

	class ScopedTimerIncrementor {
		Timing *m_parent;
	public: 
		ScopedTimerIncrementor(Timing *parent) : 
			m_parent( parent ) 
		{
			m_parent ->measure_( );
		}
		~ScopedTimerIncrementor() {
			m_parent ->incrementTime_( );
		}
		return_t get() {
			return { m_parent ->m_t, m_parent ->m_lastDeltaTime };
		}
	};

public:
	Timing() :
		m_lastClockTimePoint( now( ) )
	{}
	ScopedTimerIncrementor createAutoMeasurerScoped() { 
		return ScopedTimerIncrementor( this );
	}
};
} // namespace syscross::TraffModel
