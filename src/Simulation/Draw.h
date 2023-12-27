// src\Simulation\Draw.h - drawing tools
namespace syscross::TraffModel::Sim {
class Draw {
	const uint m_width, m_height;
	const int c_zoom = 5;
	const QPoint c_offsetScreen{ 0, 0 };
	QPointF convert_(QPointF const& point) const {
		return QPointF( 
				m_width / 2 + ( point.x( ) + c_offsetScreen.x( ) ) * c_zoom
				, m_height / 2 + ( point.y( ) + c_offsetScreen.y( ) ) * c_zoom
			);
	}

public:
	Draw(uint width, uint height) :
		m_width( width )
		, m_height( height )
	{}
	auto rotated_box(QPointF pos, QPointF size, qreal cos, qreal sin, bool centered) {
        qreal x = pos.x( ), y = pos.y( );
        qreal l = size.x( ), h = size.y( );
		auto lambdaVertex = [&](qreal e1, qreal e2) {
				return QPointF( 
						x + (e1 * l * cos + e2 * h * sin) / 2
						, y + (e1 * l * sin - e2 * h * cos) / 2
					);
			};
		std::vector< QPointF > points( 4 );
		if ( centered ) {
			points[ 0 ] = convert_( lambdaVertex( -1, -1 ) );
			points[ 1 ] = convert_( lambdaVertex( -1, +1 ) );
			points[ 2 ] = convert_( lambdaVertex( +1, +1 ) );
			points[ 3 ] = convert_( lambdaVertex( +1, -1 ) );
		} else {
			points[ 0 ] = convert_( lambdaVertex( +0, -1 ) );
			points[ 1 ] = convert_( lambdaVertex( +0, +1 ) );
			points[ 2 ] = convert_( lambdaVertex( +2, +1 ) );
			points[ 3 ] = convert_( lambdaVertex( +2, -1 ) );
		}
		return points;
	}
};
} // namespace syscross::TraffModel::Sim
