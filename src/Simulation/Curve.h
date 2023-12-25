// src\Simulation\Curve.h - 
namespace syscross::TraffModel::Sim {
struct Curve {
	// @from https://www.codeconvert.ai/python-to-c++-converter
	static const int TURN_LEFT = 0;
	static const int TURN_RIGHT = 1;
	static std::vector< QPointF > curve_points(QPoint start, QPoint end, QPointF control, int resolution=5) {
		if ((start.x( ) - end.x( )) * (start.y( ) - end.y( )) == 0) {
			return {start, end};
		}
		std::vector< QPointF > path;
		for (int i = 0; i <= resolution; i++) {
			double t = static_cast<double>(i) / resolution;
			double x = std::pow(1 - t, 2) * start.x( ) + 2 * (1 - t) * t * control.x( ) + std::pow(t, 2) * end.x( );
			double y = std::pow(1 - t, 2) * start.y( ) + 2 * (1 - t) * t * control.y( ) + std::pow(t, 2) * end.y( );
			path.push_back( QPointF( x, y ));
		}
		return path;
	}
	static std::vector< Road > curve_road(QPoint start, QPoint end, QPointF control, int resolution=15) {
		std::vector< QPointF > points = curve_points( start, end, control, resolution );
		std::vector< Road > road;
		for (int i = 1; i < points.size(); i++) {
			road.push_back( { points[i - 1], points[i] } );
		}
		return road;
	}
	static std::vector< Road > turn_road(QPoint start, QPoint end, int turn_direction, int resolution=15) {
		double x = std::min(start.x( ), end.x( ));
		double y = std::min(start.y( ), end.y( ));
		QPointF control;
		if (turn_direction == TURN_LEFT) 
			control = { x - y + start.y( ), y - x + end.x( ) };
		else 
			control = { x - y + end.y( ), y - x + start.x( ) };
		return curve_road(start, end, control, resolution);
	}
};
} // namespace syscross::TraffModel::Sim
