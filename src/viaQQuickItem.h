// src\viaQQuickItem.h - render via OpenGL, using QGuiApplication+Qml+QQuickItem, without qrc, render in separate thread
namespace syscross::TraffModel {
struct viaQQuickItem { static void run(int argc = 0, char* argv[] = nullptr) { 
		//qputenv( "QSG_RENDER_LOOP", "basic" ); // @from https://www.mimec.org/blog/render-loops-and-timers-in-qtquick
		//qputenv( "QT_SCALE_FACTOR", QByteArray( "3" ) ); // @from https://stackoverflow.com/questions/77454174/drawing-qpolygonf-to-qsggeometry
		QGuiApplication app( argc, argv );
		// antialiasing/multisampling @insp https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes or QML#antialiasing:true
		QSurfaceFormat format;
		format.setSamples( 8 );
		QSurfaceFormat::setDefaultFormat( format );
		//QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );

		QQmlApplicationEngine engine; 
		using namespace Via::QuickItem;
		RegistratorLoader< MainQQuickItem > foo( 
			&engine, &app, "MainQQuickItem", "qrc:/main.qml" );
		//// force update 
		//// @insp https://github.com/huangqinjin/CMakeQt/blob/master/main.cpp
		//const auto qQuickWindow = qobject_cast<QQuickWindow*>( engine.rootObjects( ).front( ) );
		//if ( !qQuickWindow ) 
		//	return;
		//// @insp https://stackoverflow.com/questions/20800850/how-to-access-a-nested-qml-object-from-c
		//MainQQuickItem *childObject = qQuickWindow ->findChild<MainQQuickItem*>( "TraffModelQQuickItem" );
		//// @insp https://stackoverflow.com/questions/19455518/periodically-redraw-qquickitem
		//qQuickWindow->connect(qQuickWindow, &QQuickWindow::frameSwapped, qQuickWindow, [&] {
		//		childObject ->update( );
		//	});

		app.exec( );
	}
};
} // namespace syscross::TraffModel
