// src\Via\GraphicsView\MainQGraphicsView.h - main loop in main thread
namespace syscross::TraffModel::Via::GraphicsView {
class MainQGraphicsView final : public LoopLauncherQGraphicsView {
	W_OBJECT( MainQGraphicsView ) //Q_OBJECT
	using LoopLauncherQGraphicsView::LoopLauncherQGraphicsView;

	std::vector< QGraphicsPolygonItem * > m_vehiclesItems, m_trafficSignalItems;
	//std::vector< std::unique_ptr< Sim::Vehicle > > m_singleVehicleObject; // tmp

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

			// +TODO(alex): to separate class `Xxx`
			Common::init( );
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
	}
};
W_OBJECT_IMPL( MainQGraphicsView ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::GraphicsView
