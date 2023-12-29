// src\viaQQuickPaintedItem.h - using QQuickPaintedItem, render in separate thread
namespace syscross::TraffModel {
struct viaQQuickPaintedItem { static void run(int argc = 0, char* argv[] = nullptr) { 
		QGuiApplication app( argc, argv );
		//QQuickWindow::setSceneGraphBackend( QSGRendererInterface::OpenGL );

		QQmlApplicationEngine engine; 
		using namespace Via::QuickPaintedItem;
		RegistratorLoader< MainQQuickPaintedItem > foo( 
			&engine, &app, "MainQQuickPaintedItem", "qrc:/paint.qml" );

		app.exec( );
	}
};
} // namespace syscross::TraffModel
