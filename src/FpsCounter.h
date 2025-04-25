#pragma once // src\FpsCounter.h - indeed
namespace syscross::TraffModel {
class FpsCounter {
	typedef std::chrono::steady_clock clock_t;
	static constexpr auto now = &clock_t::now;
	static constexpr auto m_duration = std::chrono::seconds{ 1 };
	clock_t::time_point m_nextFpsTime;
	size_t m_frame = 0;
	QMutex m_mutex;

public:
	FpsCounter() {
		reset( );
	}
	void reset() {
		QMutexLocker locker( &m_mutex );
		m_nextFpsTime = now( ) + m_duration;
	}
	std::optional< std::string> incrementFrame() {
		QMutexLocker locker( &m_mutex );
		++m_frame;
		if ( now( ) < m_nextFpsTime )
			return { };
		size_t fps = m_frame / static_cast< size_t >( m_duration.count( ) );
		std::string str( (std::ostringstream( ) << "FPS: " << fps ).str( ) );
		m_frame = 0;
		m_nextFpsTime = now( ) + m_duration;
		return str;
	}
};
} // namespace syscross::TraffModel
