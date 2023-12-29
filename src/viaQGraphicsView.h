// src\viaQGraphicsView.h - render via QGraphicsView, render in main thread
namespace syscross::TraffModel {
struct viaQGraphicsView { static void run(int argc = 0, char* argv[] = nullptr) { 
		//QLoggingCategory::setFilterRules( "qt.scenegraph.general=true" );
		QApplication app( argc, argv );

		using namespace Via::GraphicsView;
		MainWindow mainWindow;
		mainWindow.show( );

		app.exec( );
	}
};
} // namespace syscross::TraffModel
