// src\viaQQuickPaintedItem.h - using QQuickView, QWidget::createWindowContainer, render in separate thread
namespace syscross::TraffModel {
class MyQQuickPaintedItem : public QQuickPaintedItem {
	W_OBJECT( MyQQuickPaintedItem ) //Q_OBJECT

	Sim::Road::roads_t m_roads;
	static const uint c_vehicleRate = 15; // 35;
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;
	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;
	uint m_vehiclesOnMap = 0;
	Timing m_timing;
	std::unique_ptr< Updater > m_update;
	std::unique_ptr< Scener > m_scener;

	// TODO(alex): makeme
	bool m_bDirty = true;
	// override blocks call updatePaintNode()
    void paint(QPainter *painter) override {
		// QPainter::save() + QPainter::restore()
		if ( !m_bDirty )
			return;
		if ( !m_vehicleGenerator ) {
			m_roads = Sim::AllRoads::get( );			
			// TODO(alex): to separate class `Xxx`
			auto allPaths = Sim::AllRoads::getAllPaths( );
			// add_generator
			Sim::AllRoads::inboundRoads_t inboundRoads;
			// @from https://www.codeconvert.ai/python-to-c++-converter
			for ( auto &path : allPaths ) {
				uint road_index = path.second[ 0 ];
				inboundRoads.insert( { road_index, &m_roads[ road_index ] } );
			}
			m_vehicleGenerator = std::make_unique< Sim::VehicleGenerator >( 
				c_vehicleRate, allPaths, inboundRoads );

			// add_traffic_signal
			Sim::Road::TrafficSignal::signalRoads_t signalRoads;
			Sim::AllRoads::signalIdxRoads_t signalIdxRoads = 
				Sim::AllRoads::getSignalIdxRoads( );
			for ( auto const& pair : signalIdxRoads ) 
				signalRoads.push_back( { &m_roads[ pair[ 0 ] ], &m_roads[ pair[ 1 ] ] } );
			m_trafficSignal = std::make_unique< Sim::Road::TrafficSignal >( 
				signalRoads );

			m_update = std::make_unique< Updater >( &m_roads, m_trafficSignal.get( ) );
			m_scener = std::make_unique< Scener >( &m_roads, m_trafficSignal.get( ) );
		}

		//// scroll @insp https://stackoverflow.com/questions/27233446/transform-coordinates-in-a-qquickpainteditem
		//painter ->translate( QPoint{ 50, 50 } );
		//// zoom
		//painter ->scale( 2, 2 );

		// Static objects
		painter ->setRenderHint( QPainter::Antialiasing, true );
		painter ->setBrush( { Qt::gray, Qt::SolidPattern } );
		painter ->setPen( Qt::gray );
		auto polygons = Sim::AllRoads::calc( );
		for ( auto const& polygon : polygons )
			painter ->drawPolygon( polygon );

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		QColor color = Qt::blue;
		painter ->setBrush( color );
		painter ->setPen( color );
		m_scener ->drawVehicles( [this, &color, painter](QPolygonF const& polygons) {
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
	}

	QTimer m_timer;
public: 
	explicit MyQQuickPaintedItem(QQuickItem *parent = nullptr) :
		QQuickPaintedItem( parent )
	{
		// @insp https://stackoverflow.com/questions/28728820/qgraphicsview-doesnt-always-update
		QObject::connect( &m_timer, &QTimer::timeout
				, [this] { 
					this ->update( );
				}
			);
		m_timer.start( 50 );
		//// TODO(alex): resizeable @insp https://www.qcustomplot.com/index.php/support/forum/172
		//connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
		//connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
	}
};
W_OBJECT_IMPL( MyQQuickPaintedItem ) //Q_OBJECT

struct viaQQuickPaintedItem { static void run(int argc, char* argv[]) { 
		QGuiApplication app( argc, argv );

		QQmlApplicationEngine engine; 
		qmlRegisterType< MyQQuickPaintedItem >( "MyQQuickPaintedItem", 1, 0, "MyQQuickPaintedItem" );
		// +TODO(alex): to separate class QrcLocatorReplacement
		QrcLocatorReplacement replacement( &engine );
		QUrl url( "qrc:/paint.qml" );
		QObject::connect(
				&engine
				, &QQmlApplicationEngine::objectCreated
				, &app
				, [url](QObject *obj, QUrl const& objUrl) {
					if ( !obj && url == objUrl )
						QCoreApplication::exit( -1 );
				}
				, Qt::QueuedConnection
			);
		engine.load( url );
		if ( engine.rootObjects( ).isEmpty( ) )
			return;

		app.exec( );
	}
};
} // namespace syscross::TraffModel
