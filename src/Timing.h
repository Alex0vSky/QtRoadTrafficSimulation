// src\Timing.h - solve timing task
namespace syscross::TraffModel {
class Timing { 
	typedef std::chrono::steady_clock clock_t;
	static constexpr auto now = clock_t::now;
	const clock_t::time_point c_dtZero;
	static constexpr auto c_xxxInSecond = std::chrono::duration_cast
		< std::chrono::microseconds >
		( std::chrono::seconds{ 1 } ).count( );
	const timer_t c_fixedDeltaTime = 0.08;
	clock_t::time_point m_lastClockTimePoint = { };
	timer_t m_t = 0, m_tSpeeded = 0;
	qreal m_speed = 3;
	timer_t m_lastDeltaTime = 0;

	void incrementTime_() { 
		m_t += m_lastDeltaTime;
	}
	timer_t measuredDeltaT_() { 
		auto timePoint = now( );
		auto durationFrame = std::chrono::duration_cast
			< std::chrono::microseconds >
			( timePoint - this ->m_lastClockTimePoint );
		this ->m_lastClockTimePoint = timePoint;
		this ->m_lastDeltaTime = static_cast< timer_t >( durationFrame.count( ) ) / c_xxxInSecond;
		this ->m_tSpeeded += this ->m_lastDeltaTime * this ->m_speed;
		return this ->m_lastDeltaTime;
	}
	class ScopedTimerIncrementor {
		Timing *m_parent;
	public: 
		ScopedTimerIncrementor(Timing *parent) : 
			m_parent( parent ) 
		{
			m_parent ->measuredDeltaT_( );
		}
		~ScopedTimerIncrementor() {
			m_parent ->incrementTime_( );
		}
		timer_t fixedDeltaT() { 
			return m_parent ->m_lastClockTimePoint = now( ), 0.08;
		}
		qreal getSpeed() const { 
			return m_parent ->m_speed;
		}
		timer_t getTimeSpeeded() const { 
			return m_parent ->m_tSpeeded;
		}
		std::tuple<timer_t, timer_t> getMeasured() {
			return { m_parent ->m_tSpeeded, m_parent ->m_lastDeltaTime * m_parent ->m_speed };
		}
		std::tuple<timer_t, timer_t> getFixed() {
			return { m_parent ->m_tSpeeded, m_parent ->c_fixedDeltaTime * m_parent ->m_speed };
		}

	};
public:
	ScopedTimerIncrementor createAutoMeasurer() { 
		return ScopedTimerIncrementor( this );
	}
};
} // namespace syscross::TraffModel
