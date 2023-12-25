#include "stdafx.h"
#include "HelloQt.h"
#include "Simulation/IVehicle.h"
#include "Simulation/Road.h"
#include "Simulation/Curve.h"
#include "Simulation/Draw.h"
#include "Simulation/AllRoads.h"
#include "Simulation/Vehicle.h"
#include "Simulation/VehicleGenerator.h"
#include "Info.h"
#include "Via/GraphicsView/BaseQGraphicsView.h"
#include "Via/GraphicsView/ZoomableQGraphicsView.h"
#include "Via/GraphicsView/DraggableQGraphicsView.h"
#include "viaQGraphicsView.h"
#include "viaQml.h"
#include "viaQQuickPaintedItem.h"

int WinMain(HINSTANCE, HINSTANCE, char*, int) { //int main(int argc, char* argv[]) {
	HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//::SetPriorityClass( ::GetCurrentProcess( ), HIGH_PRIORITY_CLASS );
	//_CrtSetBreakAlloc( 5886 );
	new char[]{ "Goodbye!" };
	using namespace syscross::TraffModel;
	viaQGraphicsView::run( 0, nullptr );
	//viaQml::run( 0, nullptr );
	//viaQQuickPaintedItem::run( 0, nullptr );
	return 0;
}
