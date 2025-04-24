#pragma once // src\Via\GraphicsView\MainQGraphicsView.h - main loop in main thread
#include "FpsCounter.h"
namespace syscross::TraffModel::Via::GraphicsView {
class MainQGraphicsView final : public LoopLauncherQGraphicsView {
	using LoopLauncherQGraphicsView::LoopLauncherQGraphicsView;

	std::vector< QGraphicsPolygonItem * > m_vehiclesItems, m_trafficSignalItems;
	//std::vector< std::unique_ptr< Sim::Vehicle > > m_singleVehicleObject; // tmp
	QGraphicsSimpleTextItem * m_simpleTextItem = nullptr;
	FpsCounter m_fps;

	// clear previous
	void sceneItemsErase_(std::vector< QGraphicsPolygonItem * > *items) {
		if ( items ->empty( ) )
			return;
		auto it = std::remove_if( 
				items ->begin( ), items ->end( )
				, [this](QGraphicsPolygonItem *p) {
					return scene( ) ->removeItem( p ), delete p, true;
				}
			);
		items ->erase( it, items ->end( ) );
	}

	void loop() override {
		if ( !m_vehicleGenerator ) {
			//qDebug( ) << this ->metaObject( ) ->className( );

			// Scene static elements
			auto polygons = Sim::AllRoads::calc( );
			QRectF sceneRect;
			for ( auto const& polygon : polygons ) {
				auto *item = scene( ) ->addPolygon( polygon, { Qt::gray }, Qt::gray );
				sceneRect = sceneRect.united( item ->boundingRect( ) );
			}
			this ->setSceneRect( sceneRect );

			Common::init( );
			m_simpleTextItem = scene( ) ->addSimpleText( "FPS" );
			m_fps.reset( );
		}

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		sceneItemsErase_( &m_vehiclesItems );
		m_vehiclesItems = m_scener ->drawVehicles( 
			[this](QPolygonF const& polygons) {
				return scene( ) ->addPolygon( polygons, { Qt::blue }, Qt::blue );
			} );

		m_update ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_update ->outOfBoundsVehicles( &m_vehiclesOnMap );

		sceneItemsErase_( &m_trafficSignalItems );
		m_trafficSignalItems = m_scener ->drawSignals( 
			[this](QPolygonF const& polygons, QColor color) {
				return scene( ) ->addPolygon( polygons, color, color );
			} );
		m_update ->trafficSignals( t );

		if ( auto fps = m_fps.incrementFrame( ) )
			m_simpleTextItem ->setText( fps ->c_str( ) );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
