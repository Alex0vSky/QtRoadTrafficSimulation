// src\viaQQuickPaintedItem.h - using QQuickView, QWidget::createWindowContainer, render in separate thread
namespace syscross::TraffModel {
class MyQQuickPaintedItem : public QQuickPaintedItem {
	W_OBJECT( MyQQuickPaintedItem ) //Q_OBJECT

	bool m_bDirty = true;
    void paint(QPainter *painter) override {
		// QPainter::save() + QPainter::restore()
		qDebug( ) << "paintEvent tid: " << QThread::currentThreadId( );
		if ( !m_bDirty )
			return;
		auto colorGrey = QColor::fromRgb( 180, 180, 220 );
		auto colorRed = QColor::fromRgb( 255, 0, 0 );
		QBrush brush( colorGrey, Qt::SolidPattern );
		QPen pen( brush, 0.5 ); pen.setColor( colorRed );

		//// scroll @insp https://stackoverflow.com/questions/27233446/transform-coordinates-in-a-qquickpainteditem
		//painter ->translate( QPoint{ 50, 50 } );
		//// zoom
		//painter ->scale( 2, 2 );

		//painter ->setRenderHint(QPainter::Antialiasing, false);
		painter ->setBrush( brush );
		painter ->setPen( pen );
		//poly << QPointF(-10, 10) << QPointF(10, 50) << QPointF(30, 70 )<< QPointF(60, 50) << QPointF(50, 10);
		auto polygons = Sim::AllRoads::calc( width( ), height( ) );
		for ( auto polygon : polygons )
			painter ->drawPolygon( polygon );
	}
	//QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override {
	//	qDebug( ) << "updatePaintNode tid: " << QThread::currentThreadId( );
	//	QSGSimpleRectNode *n = static_cast<QSGSimpleRectNode *>(oldNode);
	//	if ( !n ) {
	//		n = new QSGSimpleRectNode( );
	//		n ->setColor( Qt::red );
	//		n ->setRect( boundingRect( ) );
	//	}
	//	return n;
	//}

public: 
	//// Got from QtCreator
	//ui ->setupUi( this );
	//// My widget is central
	//ui ->graphicsView = new Canvas( this );
	//ui ->graphicsView ->setObjectName(QString::fromUtf8("graphicsView"));
	//this ->setCentralWidget( ui ->graphicsView );
	//QGraphicsScene *scene = ui ->graphicsView ->scene( );
	explicit MyQQuickPaintedItem(QQuickItem *parent = nullptr) :
		QQuickPaintedItem( parent )
	{
		//this ->setScale( 0.5 ); // setContentsScale
		//this ->mapRectToScene( { 1, 1, 50, 50 } );
		//this ->mapFromScene( { 50, 50 } );
		//this ->mapToScene( { 50, 50 } );
		auto rect = contentsBoundingRect( );
		QPointF point{ this ->width( ), this ->height( ) };
		//this ->transform( );
		__nop( );
	}
};
W_OBJECT_IMPL( MyQQuickPaintedItem ) //Q_OBJECT

struct viaQQuickPaintedItem { static void run(int argc, char* argv[]) { 

		//QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );
		QGuiApplication app( argc, argv );
		//// antialiasing/multisampling @insp https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes or QML#antialiasing:true
		//QSurfaceFormat format;
		//format.setSamples( 8 );
		//QSurfaceFormat::setDefaultFormat( format );
		////QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );

		QQmlApplicationEngine engine; 
		qmlRegisterType< MyQQuickPaintedItem >( "MyQQuickPaintedItem", 1, 0, "MyQQuickPaintedItem" );
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
		qDebug( ) << "main tid: " << QThread::currentThreadId( );

		// @insp https://github.com/huangqinjin/CMakeQt/blob/master/main.cpp
		const auto qQuickWindow = qobject_cast<QQuickWindow*>( engine.rootObjects( ).front( ) );
		if ( !qQuickWindow ) 
			return;
		qQuickWindow ->resize( 1000, 630 );
		app.exec( );
	}
};
} // namespace syscross::TraffModel
