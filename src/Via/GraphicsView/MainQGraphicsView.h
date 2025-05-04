#pragma once // src\Via\GraphicsView\MainQGraphicsView.h - main loop in main thread
#include "FpsCounter.h"
namespace syscross::TraffModel::Via::GraphicsView {
/**
 * Main QGraphicsView implementation for traffic simulation
 */
class MainQGraphicsView final : public LoopLauncherQGraphicsView {
	using LoopLauncherQGraphicsView::LoopLauncherQGraphicsView;

	/// Vehicle visual items
	std::vector< QGraphicsPolygonItem * > m_vehiclesItems;

	/// Traffic signal visual items
	std::vector< QGraphicsPolygonItem * > m_trafficSignalItems;

	/// FPS display text item
	QGraphicsSimpleTextItem * m_simpleTextItem = nullptr;

	/// FPS counter utility
	FpsCounter m_fps;

	//std::vector< std::unique_ptr< Sim::Vehicle > > m_singleVehicleObject; // tmp

	/**
	 * @brief Clears scene items safely
	 *
	 * Removes and deletes:
	 * - Specified graphics items
	 * - Associated scene references
	 *
	 * @param items Container of items to clear
	 */
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

	/**
	 * @brief Updates simulation visualization
	 *
	 * Handles:
	 * - Road network rendering
	 * - Vehicle position updates
	 * - Traffic signal state changes
	 * - FPS counter display
	 */
	void loop() override {
		// Initialize scene if first run
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

		// Process simulation timing
		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		// Update vehicles visualization
		sceneItemsErase_( &m_vehiclesItems );
		m_vehiclesItems = m_scener ->drawVehicles(
			[this](QPolygonF const& polygons) {
				return scene( ) ->addPolygon( polygons, { Qt::blue }, Qt::blue );
			} );

		m_updater ->roads( t, dt );

		// Handle vehicle spawning
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_updater ->outOfBoundsVehicles( &m_vehiclesOnMap );

		// Update traffic signals
		sceneItemsErase_( &m_trafficSignalItems );
		m_trafficSignalItems = m_scener ->drawSignals(
			[this](QPolygonF const& polygons, QColor color) {
				return scene( ) ->addPolygon( polygons, color, color );
			} );
		m_updater ->trafficSignals( t );

		// Update FPS display
		if ( auto fps = m_fps.incrementFrame( ) )
			m_simpleTextItem ->setText( fps ->c_str( ) );
	}
};
} // namespace syscross::TraffModel::Via::GraphicsView
