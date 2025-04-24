// src\Via\QuickPaintedItem\MainQQuickPaintedItem.h - main loop in separate thread
#include "Via/QuickPaintedItem/BaseQQuickPaintedItem.h"
#include "Via/QuickPaintedItem/ZoomableQQuickPaintedItem.h"
#include "Via/QuickPaintedItem/DraggableQQuickPaintedItem.h"
namespace syscross::TraffModel::Via::QuickPaintedItem {
class MainQQuickPaintedItem : public DraggableQQuickPaintedItem {
	FpsCounter m_fps;
	std::string m_stringFps;

	// override blocks call to updatePaintNode()
    void paint(QPainter *painter) override {
		if ( !m_vehicleGenerator ) {
			Common::init( );
			m_stringFps = "FPS";
			m_fps.reset( );
		}
		painter ->setRenderHint( QPainter::Antialiasing, true );

		DraggableQQuickPaintedItem::handleDrag( painter );
		ZoomableQQuickPaintedItem::handleZoom( painter );

		// Static objects
		painter ->setBrush( { Qt::gray, Qt::SolidPattern } );
		painter ->setPen( Qt::gray );
		auto polygons = Sim::AllRoads::calc( );
		for ( auto const& polygon : polygons )
			painter ->drawPolygon( polygon );

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		painter ->setBrush( Qt::blue );
		painter ->setPen( Qt::blue );
		m_scener ->drawVehicles( [this, painter](QPolygonF const& polygons) {
				painter ->drawPolygon( polygons );
				return nullptr;
			} );
		m_update ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_update ->outOfBoundsVehicles( &m_vehiclesOnMap );
		m_scener ->drawSignals( [this, &painter](QPolygonF const& polygons, QColor color) {
				painter ->setBrush( color );
				painter ->setPen( color );
				painter ->drawPolygon( polygons );
				return nullptr;
			} );
		m_update ->trafficSignals( t );

		if ( auto fps = m_fps.incrementFrame( ) )
			m_stringFps = fps.value( );
		painter ->setPen( Qt::black );
		painter ->drawText( QPoint{ 10, 10 }, m_stringFps.c_str( ) );

		// Smooth animation
		update( );
	}
};
} // namespace syscross::TraffModel::Via::QuickPaintedItem
