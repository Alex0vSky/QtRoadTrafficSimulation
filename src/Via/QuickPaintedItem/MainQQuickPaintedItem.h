// src\Via\QuickPaintedItem\MainQQuickPaintedItem.h - main loop in separate thread
#include "Via/QuickPaintedItem/BaseQQuickPaintedItem.h"
#include "Via/QuickPaintedItem/ZoomableQQuickPaintedItem.h"
#include "Via/QuickPaintedItem/DraggableQQuickPaintedItem.h"
namespace syscross::TraffModel::Via::QuickPaintedItem {
/**
 * QQuickPaintedItem implementation for traffic simulation rendering
 */
class MainQQuickPaintedItem : public DraggableQQuickPaintedItem {
	/// FPS counter utility
	FpsCounter m_fps;

	/// Current FPS display text
	std::string m_stringFps;

	/**
	 * @brief Renders simulation frame
	 *
	 * Processes:
	 * - Road network visualization
	 * - Vehicle position updates
	 * - Traffic signal states
	 * - Performance metrics overlay
	 * Override to block call to updatePaintNode()
	 *
	 * @param painter Rendering context
	 */
	void paint(QPainter *painter) override {
		// Configure painter antialiasing
		painter ->setRenderHint( QPainter::Antialiasing, true );

		// Apply drag transformation
		DraggableQQuickPaintedItem::handleDrag( painter );
		// Apply zoom transformation
		ZoomableQQuickPaintedItem::handleZoom( painter );

		// Render static road network
		painter ->setBrush( { Qt::gray, Qt::SolidPattern } );
		painter ->setPen( Qt::gray );
		auto polygons = Sim::AllRoads::calc( );
		for ( auto const& polygon : polygons )
			painter ->drawPolygon( polygon );

		// Process simulation timing
		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		// Render vehicles
		painter ->setBrush( Qt::blue );
		painter ->setPen( Qt::blue );
		m_scener ->drawVehicles( [this, painter](QPolygonF const& polygons) {
				painter ->drawPolygon( polygons );
				return nullptr;
			} );

		// Update vehicle positions
		m_updater ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_updater ->outOfBoundsVehicles( &m_vehiclesOnMap );

		// Render traffic signals
		m_scener ->drawSignals( [this, &painter](QPolygonF const& polygons, QColor color) {
				painter ->setBrush( color );
				painter ->setPen( color );
				painter ->drawPolygon( polygons );
				return nullptr;
			} );
		m_updater ->trafficSignals( t );

		// Update FPS display
		if ( auto fps = m_fps.incrementFrame( ) )
			m_stringFps = fps.value( );
		painter ->setPen( Qt::black );
		painter ->drawText( QPoint{ 10, 10 }, m_stringFps.c_str( ) );

		// Request next frame, smooth animation
		update( );
	}

public:
	/**
	 * @brief Constructs draggable item
	 * @param parent Optional parent QQuickItem
	 */
	explicit MainQQuickPaintedItem(QQuickItem *parent = nullptr) :
		DraggableQQuickPaintedItem( parent )
	{
		Common::init( );
		m_stringFps = "FPS";
		m_fps.reset( );
	}
};
} // namespace syscross::TraffModel::Via::QuickPaintedItem
