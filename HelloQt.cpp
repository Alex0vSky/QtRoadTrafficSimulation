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
#include "Common.h"
#include "QrcLocatorReplacement.h"
#include "RegistratorLoader.h"
#include "FpsCounter.h"
#include "Via/GraphicsView/MainWindow.h"
#include "Via/QuickItem/MainQQuickItem.h"
#include "Via/QuickPaintedItem/MainQQuickPaintedItem.h"
#include "viaQGraphicsView.h"
#include "viaQQuickItem.h"
#include "viaQQuickPaintedItem.h"

#if defined( _WIN32 ) || defined( __CYGWIN__ ) 
int WinMain(HINSTANCE, HINSTANCE, char*, int) { 
	HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 45198 );
	new char[]{ "Goodbye!" };
#else
int main(int argc, char* argv[]) {
#endif
	using namespace syscross::TraffModel;
	//viaQGraphicsView::run( );
	//viaQQuickItem::run( );
	viaQQuickPaintedItem::run( );
	return 0;
}
