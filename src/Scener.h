// src\Scener.h - scene working
namespace syscross::TraffModel {
class Scener { 
	using Road = Sim::Road;
	Road::roads_t *m_roads;
	Sim::Draw m_draw;
	Road::TrafficSignal *m_trafficSignal;
	typedef std::vector< QGraphicsPolygonItem * > return_t;

public:
	Scener(Road::roads_t *roads, Road::TrafficSignal *trafficSignal) : 
		m_roads( roads )
		, m_trafficSignal( trafficSignal ) 
	{}
	// _draw_vehicles
	return_t drawVehicles(std::function<QGraphicsPolygonItem *(QPolygonF const&)> callback) {
		return_t items;
		for ( auto & road : (*m_roads) ) {
			Sim::IVehicle::vehicles_t const& vehicles = road.getVehicles( );
			for ( auto & vehicle : vehicles ) {
				qreal l = vehicle ->length( ), h = vehicle ->width( );
				qreal sin = road.angle_sin( ), cos = road.angle_cos( );
				qreal x = road.start( ).x( ) + cos * vehicle ->x( );
				qreal y = road.start( ).y( ) + sin * vehicle ->x( );
				auto points = m_draw.rotated_box( { x, y }, { l, h }, cos, sin, true );
				QPolygonF polygons;
				for ( auto const& elem : points )
					polygons << elem;
				QGraphicsPolygonItem *vehicleItem = callback( polygons );
				if ( vehicleItem )
					items.push_back( vehicleItem );
			}
		}
		return items;
	}
	// _draw_signals
	return_t drawSignals(std::function<QGraphicsPolygonItem *(QPolygonF const&, QColor)> callback) {
		return_t items;
		Sim::Road::TrafficSignal::signalRoads_t signalRoads = 
			m_trafficSignal ->getRoads( );
		for ( uint i = 0; i < signalRoads.size( ); ++i ) {
			QColor color;
			auto currentCycle = m_trafficSignal ->getCurrentCycle( );
			const std::array<bool, 2> stat{ false, false };
			if ( stat == currentCycle ) {
				uint currentCycleIndex = m_trafficSignal ->currentCycleIndex( );
				auto cycle = m_trafficSignal ->cycle( );
				if ( cycle[ currentCycleIndex - 1 ][ i ] )
					color = Qt::yellow;
				else 
					color = Qt::red;
			} else {
				if ( currentCycle[ i ] )
					color = Qt::green;
				else
					color = Qt::red;
			}
            for ( auto &road : m_trafficSignal ->getRoads( )[i] ) {
                qreal a = 0;
                QPointF position {
						(1 - a) * road ->end( ).x( ) + a * road ->start( ).x( )
						, (1 - a) * road ->end( ).y( ) + a * road ->start( ).y( )
					};
				auto points = m_draw.rotated_box( position, { 1, 3 },
                                    road ->angle_cos( ), road ->angle_sin( ), true );
				QPolygonF polygons;
				for ( auto const& elem : points )
					polygons << elem;
				
				QGraphicsPolygonItem *vehicleItem = callback( polygons, color );
				if ( vehicleItem )
					items.push_back( vehicleItem );
			}
		}
		return items;
	}
};
} // namespace syscross::TraffModel
