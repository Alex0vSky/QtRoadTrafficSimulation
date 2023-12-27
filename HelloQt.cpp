#include "stdafx.h"
#include "HelloQt.h"
#include "Timing.h"
#include "Simulation/IVehicle.h"
#include "Simulation/Road.h"
#include "Simulation/Curve.h"
#include "Simulation/Draw.h"
#include "Simulation/AllRoads.h"
#include "Simulation/Vehicle.h"
#include "Simulation/VehicleGenerator.h"
#include "Updater.h"
#include "Scener.h"
#include "Via/GraphicsView/BaseQGraphicsView.h"
#include "Via/GraphicsView/ZoomableQGraphicsView.h"
#include "Via/GraphicsView/DraggableQGraphicsView.h"
#include "Via/GraphicsView/LoopLauncherQGraphicsView.h"
#include "Via/GraphicsView/MainQGraphicsView.h"
#include "Via/GraphicsView/MainWindow.h"
#include "viaQGraphicsView.h"
#include "viaQQuickItem.h"
#include "viaQQuickPaintedItem.h"

#if defined( _WIN32 ) || defined( __CYGWIN__ ) 
int WinMain(HINSTANCE, HINSTANCE, char*, int) { 
	HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( Xxx );
	new char[]{ "Goodbye!" };
#else
int main(int argc, char* argv[]) {
#endif
	using namespace syscross::TraffModel;
	//viaQGraphicsView::run( 0, nullptr );
	viaQQuickItem::run( 0, nullptr );
	//viaQQuickPaintedItem::run( 0, nullptr );
	return 0;
}
