#include "stdafx.h"
#if defined(_WIN32) || defined(__CYGWIN__) 
#	define A0S_SetThreadName( name ) ::SetThreadDescription( ::GetCurrentThread( ), name.c_str( ) )
#else
#	define A0S_SetThreadName( name ) while( false )
#endif
#include "HelloQt.h"
#include "Simulation/IVehicle.h"
#include "Simulation/Road.h"
#include "Simulation/Curve.h"
#include "Simulation/Draw.h"
#include "Simulation/AllRoads.h"
#include "Simulation/Vehicle.h"
#include "Simulation/VehicleGenerator.h"
#include "Info.h"
#include "viaQGraphicsView.h"
#include "viaQml.h"
#include "viaQQuickPaintedItem.h"

int WinMain(HINSTANCE, HINSTANCE, char*, int) { //int main(int argc, char* argv[]) {
	HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//::SetPriorityClass( ::GetCurrentProcess( ), HIGH_PRIORITY_CLASS );
	//_CrtSetBreakAlloc( 3586 ); // QGraphicsView( parent )
	//_CrtSetBreakAlloc( 9316 ); // polygon << elem;
	//_CrtSetBreakAlloc( 5886 );
	new char[]{ "Goodbye!" };
	//viaQGraphicsView::run( 0, nullptr );
	//viaQml::run( 0, nullptr );
	viaQQuickPaintedItem::run( 0, nullptr );
	return 0;
}
