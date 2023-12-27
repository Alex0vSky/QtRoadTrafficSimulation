// src\viaQQuickItem.h - render via OpenGL, using QGuiApplication+Qml+QQuickItem, without qrc, render in separate thread
namespace syscross::TraffModel {
// @insp https://evileg.com/ru/post/296/
class MyQQuickItem : public QQuickItem {
	W_OBJECT( MyQQuickItem ) //Q_OBJECT

	// +TODO(alex): zoom to mouse pointer
	QPoint m_point;
	float m_delta = 1;
	bool m_bDeltaChanged = false;
	// @from viaQGraphicsView#Canvas
	void wheelEvent(QWheelEvent *pQEvent) override {
		QQuickItem::wheelEvent( pQEvent );
		m_point = pQEvent ->position( ).toPoint( );
		m_delta = 1.0f + pQEvent ->angleDelta( ).y( ) / 1200.0f;
		m_bDeltaChanged = true;
	}
	QCursor openHandCursor = Qt::CursorShape::OpenHandCursor;
	QCursor closedHandCursor = Qt::CursorShape::ClosedHandCursor;
	float m_xTransformNodeMatrix = 0, m_yTransformNodeMatrix = 0;
	int m_xMouse = 0, m_yMouse = 0;
	bool m_bDrag = false;
	// Between mousePressEvent and mouseReleaseEvent
	void mouseMoveEvent(QMouseEvent* event) override { // -
		QQuickItem::mouseMoveEvent( event );
		m_xTransformNodeMatrix = event ->x( ) - m_xMouse;
		m_yTransformNodeMatrix = event ->y( ) - m_yMouse;
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
	}
	// Dont call base method @insp https://stackoverflow.com/questions/18864420/handling-mouse-events-on-a-qquickitem
    void mousePressEvent(QMouseEvent* event) override {
		this ->setCursor( closedHandCursor );
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
		m_bDrag = true;
    }
    void mouseReleaseEvent(QMouseEvent* event) override {
        QQuickItem::mouseReleaseEvent( event );
		this ->setCursor( openHandCursor );
		m_bDrag = false;
	}

	Sim::Road::roads_t m_roads;
	static const uint c_vehicleRate = 15; // 35;
	std::unique_ptr< Sim::VehicleGenerator > m_vehicleGenerator;
	std::unique_ptr< Sim::Road::TrafficSignal > m_trafficSignal;
	uint m_vehiclesOnMap = 0;
	Timing m_timing;
	std::unique_ptr< Updater > m_update;
	std::unique_ptr< Scener > m_scener;

	QSGNode *m_carsNode = nullptr, *m_ligthsNode = nullptr, *m_roadsNode = nullptr;

	void addPolygon_(QSGNode* node, QPolygonF const& polygon, uint mode, QColor color) {
		const QSGGeometry::AttributeSet &attribs = QSGGeometry::defaultAttributes_Point2D( );
		QSGGeometry *geometry = new QSGGeometry( attribs, polygon.size( ) );
		geometry ->setDrawingMode( mode );
		geometry ->setLineWidth( 1 );
		QSGGeometry::Point2D *vertices = geometry ->vertexDataAsPoint2D( );
		for ( auto const& point : polygon ) 
			(vertices++) ->set( point.x( ), point.y( ) );
		QSGGeometryNode *n = new QSGGeometryNode( );
//		qDebug( ) << "geometry" << geometry;
		n ->setGeometry( geometry );
		n ->setFlag( QSGNode::OwnsGeometry);
		QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
		material->setColor( color );
//		qDebug( ) << "material" << material;
		n ->setMaterial( material );
		n ->setFlag( QSGNode::OwnsMaterial);
//		qDebug( ) << "n" << n;
		node ->appendChildNode( n );
	}

public:
    explicit MyQQuickItem(QQuickItem *parent = 0) : 
		QQuickItem( parent )
	{
		setFlag( QQuickItem::ItemHasContents, true );
		setAcceptedMouseButtons( Qt::AllButtons );
		setFlag( QQuickItem::ItemAcceptsInputMethod, true );
		this ->setCursor( openHandCursor );

		//// TODO(alex): resizeable @insp https://www.qcustomplot.com/index.php/support/forum/172
		//connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
		//connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
	}
	// Внимание: Крайне важно, чтобы графические операции и взаимодействие с графом сцены происходили исключительно в потоке рендеринга, в первую очередь во времяupdatePaintNode() вызов. Лучшее практическое правило — использовать только классы с префиксом «QSG» внутриQQuickItem::updatePaintNode() функция.
	// Примечание. Все классы с префиксом QSG следует использовать исключительно в потоке рендеринга графа сцены. ВидетьScene Graph and RenderingЧтобы получить больше информации.
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override {
		if ( !oldNode ) {
			oldNode = new QSGNode( );
			QColor colorRed = QColor::fromRgb( 255, 0, 0 );
			QColor colorGrey = QColor::fromRgb( 180, 180, 220 );
//static const int countPoints = 5; vertices[0].set( -10, 10 ), vertices[1].set( 10, 50 ), vertices[2].set( 30, 70 ), vertices[3].set( 60, 50 ), vertices[4].set( 50, 10 );
			m_roadsNode = new QSGNode( );
			oldNode ->appendChildNode( m_roadsNode );
			auto polygons = Sim::AllRoads::calc( );
			for ( QPolygonF const& polygon : polygons ) {
				// lower 
				addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawTriangleFan, colorGrey );
				//// upper
				//addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawLineLoop, colorRed );
			}

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

			oldNode ->appendChildNode( m_carsNode = new QSGNode( ) );
			oldNode ->appendChildNode( m_ligthsNode = new QSGNode( ) );
//			QRectF bounds = boundingRect( );
//			m_xTransformNodeMatrix = ( bounds.width( ) / 2 );
//			m_yTransformNodeMatrix = ( bounds.height( ) / 2 );
//			auto initMatrix = data ->transformNode ->matrix( );
//			initMatrix.translate( m_xTransformNodeMatrix, m_yTransformNodeMatrix );
//			data ->transformNode ->setMatrix( initMatrix );
		}

		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		while ( QSGNode* node = m_carsNode ->firstChild( ) ) 
			delete node;
		QColor color = Qt::blue;
		m_scener ->drawVehicles( [this, &color](QPolygonF const& polygons) {
				addPolygon_( m_carsNode, polygons, QSGGeometry::DrawTriangleFan, color );
				return nullptr;
			} );
		m_update ->roads( t, dt );
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_update ->outOfBoundsVehicles( &m_vehiclesOnMap );

		while ( QSGNode* node = m_ligthsNode ->firstChild( ) ) 
			delete node;
		m_scener ->drawSignals( [this, oldNode](QPolygonF const& polygons, QColor color) {
				addPolygon_( m_ligthsNode, polygons, QSGGeometry::DrawTriangleFan, color );
				return nullptr;
			} );
		m_update ->trafficSignals( t );

		QMatrix4x4 transformNodeMatrix = data ->transformNode ->matrix( );
		if ( m_bDrag )
			transformNodeMatrix.translate( m_xTransformNodeMatrix, m_yTransformNodeMatrix );
		else {
			if ( m_bDeltaChanged ) {
				m_bDeltaChanged = false;
				transformNodeMatrix.translate( m_point.x( ), m_point.y( ) );
				transformNodeMatrix.scale( m_delta );
				transformNodeMatrix.translate( -m_point.x( ), -m_point.y( ) );
			}
		}
		data ->transformNode ->setMatrix( transformNodeMatrix );

		update( );
		return oldNode;
	} 
};
W_OBJECT_IMPL( MyQQuickItem ) //Q_OBJECT

struct viaQQuickItem { static void run(int argc, char* argv[]) { 
		//qputenv( "QSG_RENDER_LOOP", "basic" ); // @from https://www.mimec.org/blog/render-loops-and-timers-in-qtquick
		//qputenv( "QT_SCALE_FACTOR", QByteArray( "3" ) ); // @from https://stackoverflow.com/questions/77454174/drawing-qpolygonf-to-qsggeometry

		QGuiApplication app( argc, argv ); //view.setResizeMode(QQuickView::SizeRootObjectToView);
		// antialiasing/multisampling @insp https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes or QML#antialiasing:true
		QSurfaceFormat format;
		format.setSamples( 8 );
		QSurfaceFormat::setDefaultFormat( format );
		//QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );

		QQmlApplicationEngine engine; 
		qmlRegisterType<MyQQuickItem>( "MyQQuickItem", 1, 0, "MyQQuickItem" );

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

		QUrl url( "qrc:/main.qml" );
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

		// @insp https://github.com/huangqinjin/CMakeQt/blob/master/main.cpp
		const auto qQuickWindow = qobject_cast<QQuickWindow*>( engine.rootObjects( ).front( ) );
		if ( !qQuickWindow ) 
			return;
		//qQuickWindow ->resize( 1000, 630 );
		//// logging
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::beforeRendering, qQuickWindow, [] {
		//    qDebug() << "before rendering";
		//}, Qt::DirectConnection);
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::afterRendering, qQuickWindow, [] {
		//    qDebug() << "after rendering";
		//}, Qt::DirectConnection);

		//// force update 
		//// @insp https://stackoverflow.com/questions/20800850/how-to-access-a-nested-qml-object-from-c
		//MyQQuickItem *childObject = qQuickWindow ->findChild<MyQQuickItem*>( "TraffModelQQuickItem" );
		//// @insp https://stackoverflow.com/questions/19455518/periodically-redraw-qquickitem
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::frameSwapped, qQuickWindow, [&] {
		//		childObject ->update( );
		//	});

		app.exec( );
	}
};
} // namespace syscross::TraffModel
