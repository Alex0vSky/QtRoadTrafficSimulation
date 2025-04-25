#pragma once // src\Scener.h - scene working
namespace syscross::TraffModel {
/**
 * Handles visualization of simulation elements:
 * - Vehicles
 * - Roads
 * - Traffic signals
 */
class Scener {
	using Road = Sim::Road;
	/// Reference to all road segments for visualization
	Road::roads_t *m_roads;
	/// Drawing utilities for rendering shapes
	Sim::Draw m_draw;

	/// Traffic light controller reference
	Road::TrafficSignal *m_trafficSignal;
	typedef std::vector< QGraphicsPolygonItem * > return_t;

public:
	/**
	 * @brief Constructs the visualizer
	 * @param roads Reference to road network
	 * @param trafficSignal Traffic light controller
	 */
	Scener(Road::roads_t *roads, Road::TrafficSignal *trafficSignal) :
		m_roads( roads )
		, m_trafficSignal( trafficSignal )
	{}

	/**
	 * @brief Renders all vehicles on roads
	 *
	 * Transforms vehicle states to visual polygons:
	 * - Handles position/orientation
	 * - Applies vehicle dimensions
	 *
	 * @param callback Rendering function for each vehicle
	 * @return Container of rendered vehicle items
	 */
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

	/**
	 * @brief Renders traffic signals
	 *
	 * Visualizes signal states:
	 * - Green/Yellow/Red colors
	 * - Position at road ends
	 *
	 * @param callback Rendering function for each signal
	 * @return Container of rendered signal items
	 */
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
