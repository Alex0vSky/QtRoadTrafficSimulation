// src\Simulation\Roads.h - 
namespace Simulation {
// TrafficSimulator\Setups\two_way_intersection.py
class AllRoads {
	static const uint c_curveResolution = 15;
	static auto turn(uint i) {
		std::vector< uint > turnIndexes;
		for ( auto _{ c_curveResolution }; _--; )
			turnIndexes.push_back( i++ );
		return turnIndexes;
	}

public:
	// tmp
	static auto W_R_S() {
		return turn( 12 );
	}

    static Road::roads_t get() {
		// Returns the sine of the angle v in radians.
		auto radianSin = qSin( 1.0 ), radianCos = qCos( 1.0 );
		auto degreesSin = qRadiansToDegrees( radianSin );

		// Short offset from (0, 0)
		int a = 2; 
		// Long offset from (0, 0)
		int b = 12; 
		// Road length
		int length = 50; 
		// Intersection offset from the center
		int offset = 25; 

		// Nodes
		QPoint WEST_RIGHT_START = { -b - length, a };
		QPoint WEST_LEFT_START = { -b - length, -a };

		QPoint SOUTH_RIGHT_START = { a, b + length };
		QPoint SOUTH_LEFT_START = { -a, b + length };

		QPoint EAST_RIGHT_START = { b + length, -a };
		QPoint EAST_LEFT_START = { b + length, a };

		QPoint NORTH_RIGHT_START = { -a, -b - length };
		QPoint NORTH_LEFT_START = { a, -b - length };

		QPoint WEST_RIGHT = { -b, a };
		QPoint WEST_LEFT = { -b, -a };

		QPoint SOUTH_RIGHT = { a, b };
		QPoint SOUTH_LEFT = { -a, b };

		QPoint EAST_RIGHT = { b, -a };
		QPoint EAST_LEFT = { b, a };

		QPoint NORTH_RIGHT = { -a, -b };
		QPoint NORTH_LEFT = { a, -b };

		// Roads
		Road WEST_INBOUND = { WEST_RIGHT_START, WEST_RIGHT };
		Road SOUTH_INBOUND = { SOUTH_RIGHT_START, SOUTH_RIGHT };
		Road EAST_INBOUND = { EAST_RIGHT_START, EAST_RIGHT };
		Road NORTH_INBOUND = { NORTH_RIGHT_START, NORTH_RIGHT };

		Road WEST_OUTBOUND = { WEST_LEFT, WEST_LEFT_START };
		Road SOUTH_OUTBOUND = { SOUTH_LEFT, SOUTH_LEFT_START };
		Road EAST_OUTBOUND = { EAST_LEFT, EAST_LEFT_START };
		Road NORTH_OUTBOUND = { NORTH_LEFT, NORTH_LEFT_START };

		Road WEST_STRAIGHT = { WEST_RIGHT, EAST_LEFT };
		Road SOUTH_STRAIGHT = { SOUTH_RIGHT, NORTH_LEFT };
		Road EAST_STRAIGHT = { EAST_RIGHT, WEST_LEFT };
		Road NORTH_STRAIGHT = { NORTH_RIGHT, SOUTH_LEFT };

		auto turn_road = Simulation::Curve::turn_road;
		auto TURN_RIGHT = Simulation::Curve::TURN_RIGHT;
		auto TURN_LEFT = Simulation::Curve::TURN_LEFT;
		//using namespace Simulation::Curve;
		std::vector< Road > WEST_RIGHT_TURN = turn_road( WEST_RIGHT, SOUTH_LEFT, TURN_RIGHT, c_curveResolution );
		std::vector< Road > WEST_LEFT_TURN = turn_road( WEST_RIGHT, NORTH_LEFT, TURN_LEFT, c_curveResolution );

		std::vector< Road > SOUTH_RIGHT_TURN = turn_road( SOUTH_RIGHT, EAST_LEFT, TURN_RIGHT, c_curveResolution );
		std::vector< Road > SOUTH_LEFT_TURN = turn_road( SOUTH_RIGHT, WEST_LEFT, TURN_LEFT, c_curveResolution );

		std::vector< Road > EAST_RIGHT_TURN = turn_road( EAST_RIGHT, NORTH_LEFT, TURN_RIGHT, c_curveResolution );
		std::vector< Road > EAST_LEFT_TURN = turn_road( EAST_RIGHT, SOUTH_LEFT, TURN_LEFT, c_curveResolution );

		std::vector< Road > NORTH_RIGHT_TURN = turn_road( NORTH_RIGHT, WEST_LEFT, TURN_RIGHT, c_curveResolution );
		std::vector< Road > NORTH_LEFT_TURN = turn_road( NORTH_RIGHT, EAST_LEFT, TURN_LEFT, c_curveResolution );

		Road::roads_t ROADS = {
				WEST_INBOUND // 0
				, SOUTH_INBOUND // 1
				, EAST_INBOUND // 2
				, NORTH_INBOUND // 3
				, WEST_OUTBOUND // 4
				, SOUTH_OUTBOUND // 5
				, EAST_OUTBOUND // 6
				, NORTH_OUTBOUND // 7

				, WEST_STRAIGHT // 8
				, SOUTH_STRAIGHT // 9
				, EAST_STRAIGHT // 10
				, NORTH_STRAIGHT // 11
			};
		for ( auto &elem : WEST_RIGHT_TURN )
			ROADS.push_back( elem );
		for ( auto &elem : WEST_LEFT_TURN )
			ROADS.push_back( elem );

		for ( auto &elem : SOUTH_RIGHT_TURN )
			ROADS.push_back( elem );
		for ( auto &elem : SOUTH_LEFT_TURN )
			ROADS.push_back( elem );

		for ( auto &elem : EAST_RIGHT_TURN )
			ROADS.push_back( elem );
		for ( auto &elem : EAST_LEFT_TURN )
			ROADS.push_back( elem );

		for ( auto &elem : NORTH_RIGHT_TURN )
			ROADS.push_back( elem );
		for ( auto &elem : NORTH_LEFT_TURN )
			ROADS.push_back( elem );

		uint index = 0;
		for ( auto & road : ROADS )
			road.setIndex( index++ );
		return ROADS;
	}

	// TODO(alex): makeme zoom
	static auto calc(uint width, uint height, uint zoom = 5) {
		Draw draw( width, height, zoom );
		float h = 3.7;
		std::vector< QPolygonF > polygons;
		Road::roads_t roads = get( );
		for ( auto &road : roads ) {
			qreal l = road.length( );
			double angle_cos = road.angle_cos( );
			double angle_sin = road.angle_sin( );
			auto points = draw.rotated_box( 
					road.start( )
					, { road.length( ), h } 
					, road.angle_cos( )
					, road.angle_sin( )
					, false
				);
			QPolygonF polygon;
			for ( auto const& elem : points )
				polygon << elem;
			polygons.push_back( polygon );
		}
		return polygons;
	}

	typedef uint weight_t;
	typedef uint roadIdx_t;
	typedef std::pair< weight_t, std::vector< roadIdx_t > > flatElem_t;
	typedef std::vector< flatElem_t > flatPathIndexes_t;

	static flatPathIndexes_t getAllPaths() {
		typedef std::vector< roadIdx_t > roadIdxOrTurnSegment_t;
		std::vector
			< 
				std::pair
					< 
						weight_t, std::tuple< roadIdx_t, roadIdxOrTurnSegment_t, roadIdx_t > 
					>
			> 
			pathIndexes {
				// WEST STRAIGHT EAST
				{ 3, { 0, { 8 }, 6 } }
				// WEST RIGHT SOUTH
				, { 1, { 0, turn( 12 ), 5 } }
				// WEST LEFT NORTH
				 //, { 1, { 0, turn( 27 ), 7 } } // collision

				// SOUTH STRAIGHT NORTH
				, { 3, { 1, { 9 }, 7 } }
				// SOUTH RIGHT EAST
				, { 1, { 1, turn( 42 ), 6 } }
				// SOUTH LEFT WEST
				//,  { 1, { 1, turn( 57 ), 4 } } // collision

				// EAST STRAIGHT WEST
				, { 3, { 2, { 10 }, 4 } }
				// EAST RIGHT NORTH
				, { 1, { 2, turn( 72 ), 7 } }
				// EAST LEFT SOUTH
				//, { 1, { 2, turn( 87 ), 5 } } // collision
	
				// NORTH STRAIGHT SOUTH
				, { 3, { 3, { 11 }, 5 } }
				// NORTH RIGHT WEST
				, { 1, { 3, turn( 102 ), 4 } }
				// NORTH LEFT EAST
				//, { 1, { 3, turn( 117 ), 6 } } // collision
			};
		typedef std::pair
					< 
						weight_t, std::vector< roadIdx_t > 
					>
			flatElem_t;
		std::vector< flatElem_t > flatPathIndexes;
		for ( auto & elem : pathIndexes ) {
			flatElem_t flatElem;
			flatElem.first = elem.first;
			std::vector< uint > second;
			uint firstRoadIdxInSegm = std::get< 0 >( elem.second );
			std::vector< uint > roadIdxOrTurn = std::get< 1 >( elem.second );
			uint lastRoadIdxInSegm = std::get< 2 >( elem.second );
			second.push_back( firstRoadIdxInSegm );
			second.insert( second.end( ), roadIdxOrTurn.begin( ), roadIdxOrTurn.end( ) );
			second.push_back( lastRoadIdxInSegm );
			flatElem.second = second;
			flatPathIndexes.push_back( flatElem );
		}
		return flatPathIndexes;
	}

	typedef std::map< uint, Road* > inboundRoads_t;

	typedef std::vector< std::array< roadIdx_t, 2 > > signalIdxRoads_t;
	static auto getSignalIdxRoads() {
		// its road::m_index
		const signalIdxRoads_t signalIdxRoads = {
				// WEST, EAST
				{ 0, 2 }
				// SOUTH NORTH
				, { 1, 3 }
			};
		return signalIdxRoads;
	}
};
} // namespace Simulation 
