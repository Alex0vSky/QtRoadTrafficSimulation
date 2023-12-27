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
		QBrush brush( Qt::gray, Qt::SolidPattern );
		QPen pen( brush, 1 );
		//pen.setColor( Qt::red );
		painter ->setRenderHint( QPainter::Antialiasing, true );
		painter ->setBrush( brush );
		painter ->setPen( pen );
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
	}
};
W_OBJECT_IMPL( MyQQuickPaintedItem ) //Q_OBJECT

struct viaQQuickPaintedItem { static void run(int argc, char* argv[]) { 
		QGuiApplication app( argc, argv );

		QQmlApplicationEngine engine; 
		qmlRegisterType< MyQQuickPaintedItem >( "MyQQuickPaintedItem", 1, 0, "MyQQuickPaintedItem" );
		// TODO(alex): to separate class QrcLocatorReplacement
		// qrc locator replacement
#pragma region
		auto currentSystemPath = QDir::currentPath( );
		auto currentBinaryPath = qApp ->applicationDirPath( );
		static const QString prefix{ "qrc" };
		QStringList searchPaths{ 
				currentSystemPath + "/../resource/qml"
				, currentBinaryPath + "/../../../resource/qml"
			};
		QDir::setSearchPaths( prefix, searchPaths );
		// engine.setBaseUrl not working or use Q_INIT_RESOURCE
		// @insp https://www.kdab.com/fun-with-paths-urls-in-qml/
		// @insp https://stackoverflow.com/questions/39701903/difference-between-foo-qrc-foo-and-qrc-foo-paths-in-qt
		auto urlInterceptorFirmware = engine.urlInterceptor( );
		class UrlInterceptor : public QQmlAbstractUrlInterceptor {
			QQmlAbstractUrlInterceptor *m_parent;
			QUrl intercept(const QUrl& path, QQmlAbstractUrlInterceptor::DataType type) override {
				do {
					if ( QQmlAbstractUrlInterceptor::QmlFile != type )
						break;
					if ( path.scheme() != "qrc" )
						break;
					QFileInfo fileInfo( prefix + ":" + path.path( ) );
					if ( !fileInfo.exists( ) )
						break;
					return QUrl::fromLocalFile( fileInfo.filePath( ) );
				} while ( false );
				return m_parent ->intercept( path, type );
			};
		public:
			UrlInterceptor(QQmlAbstractUrlInterceptor *parent) :
				m_parent( parent )
			{}
		} urlInterceptor( urlInterceptorFirmware );
		engine.setUrlInterceptor( &urlInterceptor );
#pragma endregion
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
