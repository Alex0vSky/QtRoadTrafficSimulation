// src\Scener.h - scene working
namespace syscross::TraffModel {
class Scener { 
	using Road = Sim::Road;
	QGraphicsScene *m_scene;
	Sim::Draw m_draw;
	Road::roads_t *m_roads;
	Road::TrafficSignal *m_trafficSignal;
	std::vector< QGraphicsPolygonItem * > m_vehiclesItems, m_trafficSignalItems;

public:
	Scener(QGraphicsScene *scene
		, uint width, uint height
		, Road::roads_t *roads
		, Road::TrafficSignal *trafficSignal
	) : 
		m_scene( scene )
		, m_draw( width, height )
		, m_roads( roads )
		, m_trafficSignal( trafficSignal ) 
	{}
	void drawVehicles() {
#pragma region clear previous
		auto it = std::remove_if( m_vehiclesItems.begin( ), m_vehiclesItems.end( )
				, [this](QGraphicsPolygonItem *p) {
					return m_scene ->removeItem( p ), delete p, true;
				}
			);
		m_vehiclesItems.erase( it, m_vehiclesItems.end( ) );
#pragma endregion
		QColor colorBlue = QColor::fromRgb( 0, 0, 255 );
		QBrush brush( colorBlue, Qt::SolidPattern );
		QPen pen( colorBlue );
		for ( auto & road : (*m_roads) ) {
			Sim::IVehicle::vehicles_t const& vehicles = road.getVehicles( );
			for ( auto & vehicle : vehicles ) {
				qreal l = vehicle ->length( ), h = vehicle ->width( );
				qreal sin = road.angle_sin( ), cos = road.angle_cos( );
				qreal x = road.start( ).x( ) + cos * vehicle ->x( );
				qreal y = road.start( ).y( ) + sin * vehicle ->x( );
				auto points = m_draw.rotated_box( { x, y }, { l, h }, cos, sin, true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				auto vehicleItem = m_scene ->addPolygon( polygon, pen, brush );
				m_vehiclesItems.push_back( vehicleItem );
			}
		}
	}
	// _draw_signals
	void drawSignals() {
#pragma region clear previous
		auto it = std::remove_if( m_trafficSignalItems.begin( ), m_trafficSignalItems.end( )
				, [this](QGraphicsPolygonItem *p) {
					return m_scene ->removeItem( p ), delete p, true;
				}
			);
		m_trafficSignalItems.erase( it, m_trafficSignalItems.end( ) );
#pragma endregion
		Sim::Road::TrafficSignal::signalRoads_t signalRoads = 
			m_trafficSignal ->getRoads( );
		for ( uint i = 0; i < signalRoads.size( ); ++i ) {
			QColor color;
			QColor red = QColor::fromRgb( 255, 0, 0 );
			QColor green = QColor::fromRgb( 0, 255, 0 );
			auto currentCycle = m_trafficSignal ->getCurrentCycle( );
			const std::array<bool, 2> stat{ false, false };
			bool b = ( currentCycle == stat );
			if ( b ) {
				QColor yellow = QColor::fromRgb( 255, 255, 0 );
				uint currentCycleIndex = m_trafficSignal ->currentCycleIndex( );
				auto cycle = m_trafficSignal ->cycle( );
				if ( cycle[ currentCycleIndex - 1 ][ i ] )
					color = yellow;
				else 
					color = red;
			} else {
				if ( currentCycle[ i ] )
					color = green;
				else
					color = red;
			}
            for ( auto &road : m_trafficSignal ->getRoads( )[i] ) {
                qreal a = 0;
                QPointF position {
						(1 - a) * road ->end( ).x( ) + a * road ->start( ).x( )
						, (1 - a) * road ->end( ).y( ) + a * road ->start( ).y( )
					};
				auto points = m_draw.rotated_box( position, { 1, 3 },
                                    road ->angle_cos( ), road ->angle_sin( ), true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				QPen pen( color );
				QBrush brush( color, Qt::SolidPattern );
				auto trafficSignalItem = m_scene ->addPolygon( polygon, pen, brush );
				m_trafficSignalItems.push_back( trafficSignalItem );
			}
		}
	}
};
} // namespace syscross::TraffModel
