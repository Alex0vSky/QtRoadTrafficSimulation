// src\viaQml.h - render via OpenGL, using QGuiApplication+Qml+QQuickItem, without qrc, render in separate thread
// TODO(alex): not QML, rename

// @insp https://evileg.com/ru/post/296/
class MyQQuickItem : public QQuickItem {
	W_OBJECT( MyQQuickItem ) //Q_OBJECT

	float m_delta = 1;
	// @from viaQGraphicsView#Canvas
	void wheelEvent(QWheelEvent *pQEvent) override {
		QQuickItem::wheelEvent( pQEvent );
        qDebug() << "wheelEvent: " << pQEvent ->angleDelta( );
		m_delta = 1.0f + pQEvent->angleDelta().y() / 1200.0f;
		this ->update( );
	}
	QCursor openHandCursor = Qt::CursorShape::OpenHandCursor;
	QCursor closedHandCursor = Qt::CursorShape::ClosedHandCursor;
	float m_xTransformNodeMatrix = 0;
	float m_yTransformNodeMatrix = 0;
	int m_xMouse = 0;
	int m_yMouse = 0;
	bool m_bDrag = false;
	// Between mousePressEvent and mouseReleaseEvent
	void mouseMoveEvent(QMouseEvent* event) override { // -
		QQuickItem::mouseMoveEvent( event );
        qDebug() << "mouseMoveEvent: " << event ->pos( );
		//if (event->buttons() & Qt::LeftButton)
		//	// If we are moveing with the left button down, update the scene to trigger autocompute
		//	scene()->update(mapToScene(rect()).boundingRect());
		m_xTransformNodeMatrix = event ->x( ) - m_xMouse;
		m_yTransformNodeMatrix = event ->y( ) - m_yMouse;
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
		this ->update( );
	}
	// Dont call base method @insp https://stackoverflow.com/questions/18864420/handling-mouse-events-on-a-qquickitem
    void mousePressEvent(QMouseEvent* event) override {
        qDebug() << "mousePressEvent: " << event ->pos( );
		this ->setCursor( closedHandCursor );
		m_xMouse = event ->x( );
		m_yMouse = event ->y( );
		m_bDrag = true;
    }
    void mouseReleaseEvent(QMouseEvent* event) override {
        QQuickItem::mouseReleaseEvent( event );
        qDebug() << "mouseReleaseEvent: " << event ->pos( );
		this ->setCursor( openHandCursor );
		m_bDrag = false;
	}
	// @insp https://stackoverflow.com/questions/30281955/catch-mousemoveevent-on-qt5s-qquickitem
    void hoverMoveEvent(QHoverEvent* event) override {
        QQuickItem::hoverMoveEvent(event);
        qDebug() << "hoverMoveEvent: " << event ->pos( );
    }

	void addPolygon_(QSGNode* node, QPolygonF const& polygon, unsigned int mode, QColor color) {
		const QSGGeometry::AttributeSet &attribs = QSGGeometry::defaultAttributes_Point2D( );
		QSGGeometry *geometry = new QSGGeometry( attribs, polygon.size( ) );
		geometry ->setDrawingMode( mode );
		geometry ->setLineWidth( 1 );
		QSGGeometry::Point2D *vertices = geometry ->vertexDataAsPoint2D( );
		for( auto const& point : polygon ) 
			(vertices++) ->set( point.x( ), point.y( ) );
		//(vertices++) ->set( polygon[ 0 ].x( ), polygon[ 0 ].y( ) );
		//(vertices++) ->set( polygon[ 1 ].x( ), polygon[ 1 ].y( ) );
		//(vertices++) ->set( polygon[ 2 ].x( ), polygon[ 2 ].y( ) );
		//(vertices++) ->set( polygon[ 3 ].x( ), polygon[ 3 ].y( ) );
		QSGGeometryNode *n = new QSGGeometryNode( );
		n ->setGeometry( geometry );
		n ->setFlag( QSGNode::OwnsGeometry);
		QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
		material->setColor( color );
		n ->setMaterial( material );
		n ->setFlag( QSGNode::OwnsMaterial);
		node ->appendChildNode( n );
	}

public:
    explicit MyQQuickItem(QQuickItem *parent = 0) : 
		QQuickItem( parent )
	{
		setFlag(QQuickItem::ItemHasContents, true);
		// ??? this ->mapToScene( { 100, 100 } );
		setAcceptedMouseButtons( Qt::AllButtons );
		setAcceptHoverEvents( true );
		setFlag(ItemAcceptsInputMethod, true);
		//setMouseTracking( true );
		this ->setCursor( openHandCursor );

		//// TODO(alex): resizeable @insp https://www.qcustomplot.com/index.php/support/forum/172
		//connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
		//connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
	}
	// Внимание: Крайне важно, чтобы графические операции и взаимодействие с графом сцены происходили исключительно в потоке рендеринга, в первую очередь во времяupdatePaintNode() вызов. Лучшее практическое правило — использовать только классы с префиксом «QSG» внутриQQuickItem::updatePaintNode() функция.
	// Примечание. Все классы с префиксом QSG следует использовать исключительно в потоке рендеринга графа сцены. ВидетьScene Graph and RenderingЧтобы получить больше информации.
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override {
		if ( !oldNode ) {
			oldNode = new QSGNode( );
			QColor colorRed = QColor::fromRgb( 255, 0, 0 );
			QColor colorGrey = QColor::fromRgb( 180, 180, 220 );
//static const int countPoints = 5; poly << QPointF(-10, 10) << QPointF(10, 50) << QPointF(30, 70 )<< QPointF(60, 50) << QPointF(50, 10);
			//vertices[0].set( -10, 10 );
			//vertices[1].set( 10, 50 );
			//vertices[2].set( 30, 70 );
			//vertices[3].set( 60, 50 );
			//vertices[4].set( 50, 10 );
			auto polygons = Simulation::AllRoads::calc( width( ), height( ) );
			for ( QPolygonF const& polygon : polygons ) {
				// lower 
				addPolygon_( oldNode, polygon, QSGGeometry::DrawTriangleFan, colorGrey );
				// upper
				addPolygon_( oldNode, polygon, QSGGeometry::DrawLineLoop, colorRed );
				//qDebug( ) << polygon; //// QPolygonF(QPointF(190,334.25)QPointF(190,315.75)QPointF(440,315.75)QPointF(440,334.25))
			}
//			QRectF bounds = boundingRect( );
//			m_xTransformNodeMatrix = ( bounds.width( ) / 2 );
//			m_yTransformNodeMatrix = ( bounds.height( ) / 2 );
//			auto initMatrix = data ->transformNode ->matrix( );
//			initMatrix.translate( m_xTransformNodeMatrix, m_yTransformNodeMatrix );
//			data ->transformNode ->setMatrix( initMatrix );
		}

		QMatrix4x4 transformNodeMatrix = data ->transformNode ->matrix( );
		if ( m_bDrag )
			transformNodeMatrix.translate( m_xTransformNodeMatrix, m_yTransformNodeMatrix );
		else
			transformNodeMatrix.scale( m_delta );
		data ->transformNode ->setMatrix( transformNodeMatrix );

		return oldNode;
	} 
};
W_OBJECT_IMPL( MyQQuickItem ) //Q_OBJECT

struct viaQml { static void run(int argc, char* argv[]) { 
		//qputenv( "QSG_RENDER_LOOP", "basic" ); // @from https://www.mimec.org/blog/render-loops-and-timers-in-qtquick
		//qputenv( "QT_SCALE_FACTOR", QByteArray( "3" ) ); // @from https://stackoverflow.com/questions/77454174/drawing-qpolygonf-to-qsggeometry

		QGuiApplication app( argc, argv );
		//view.setResizeMode(QQuickView::SizeRootObjectToView);

		//// antialiasing/multisampling @insp https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes or QML#antialiasing:true
		//QSurfaceFormat format;
		//format.setSamples( 8 );
		//QSurfaceFormat::setDefaultFormat( format );
		////QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );

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
		qQuickWindow ->resize( 1000, 630 );
		//// logging
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::beforeRendering, qQuickWindow, [] {
		//    qDebug() << "before rendering";
		//}, Qt::DirectConnection);
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::afterRendering, qQuickWindow, [] {
		//    qDebug() << "after rendering";
		//}, Qt::DirectConnection);
		//// @insp https://stackoverflow.com/questions/20800850/how-to-access-a-nested-qml-object-from-c
		//// force update 
		//MyQQuickItem *childObject = qQuickWindow ->findChild<MyQQuickItem*>( "SomeNumberText" );
		//// @insp https://stackoverflow.com/questions/19455518/periodically-redraw-qquickitem
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::frameSwapped, qQuickWindow, [&] {
		//		childObject ->update( );
		//	});

		app.exec( );
	}
};
