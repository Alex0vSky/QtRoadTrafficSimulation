// src\viaQGraphicsView.h - render via QGraphicsView, render in main thread

// TODO(alex): inher tree, base=>Zoomable=>Dragable=>StatusAware
class MyQGraphicsView : public QGraphicsView {
	W_OBJECT( MyQGraphicsView ) //Q_OBJECT
	QMainWindow *m_parent;
	const QPoint m_viewportTopleft = { -20, -20 };
	const QPoint m_viewportBottomRight = { 20, 20 };
	QPointF m_topLeft;
	QPointF m_bottomRight;
	QTimer m_timer;
	// @insp https://stackoverflow.com/questions/47708282/zoom-functionality-using-qt
    virtual void wheelEvent(QWheelEvent *pQEvent) override {
		//qDebug() << "Wheel Event:";
		// pos() -> virtual canvas
		QPointF point = pQEvent->position( );
		QPointF pos = mapToScene( point.toPoint( ) );
		//qDebug() << "mouse pos:" << pos;
		// scale from wheel angle
		float delta = 1.0f + pQEvent->angleDelta().y() / 1200.0f;
		//qDebug() << "angleDelta:" << pQEvent->angleDelta().y();
		m_delta += pQEvent->angleDelta().y();
		//qDebug() << "scale factor:" << delta;
		// modify transform matrix
		QTransform xform = transform();
		xform.translate(pos.x(), pos.y()); // origin to spot
		xform.scale(delta, delta); // scale
		xform.translate(-pos.x(), -pos.y()); // spot to origin
		setTransform(xform);
		//qDebug() << "transform:" << xform;
		// force update
		update( );
		pQEvent->accept( );
		showStatus( );
	}
	void showStatus() {
		QString text = QString( "scale: %1, LT: %2/%3, BR: %4/%5" )
				.arg( m_delta )
				.arg( m_topLeft.x( ) )
				.arg( m_topLeft.y( ) )
				.arg( m_bottomRight.x( ) )
				.arg( m_bottomRight.y( ) )
			;
		m_parent ->statusBar( ) ->showMessage( text );
	}

	// @insp https://stackoverflow.com/questions/55007339/allow-qgraphicsview-to-move-outside-scene/55043082
	void mouseMoveEvent(QMouseEvent* event) override {
		QGraphicsView::mouseMoveEvent(event);
		if (event->buttons() & Qt::LeftButton)
			// If we are moveing with the left button down, update the scene to trigger autocompute
			scene()->update(mapToScene(rect()).boundingRect());
	}
	void mousePressEvent(QMouseEvent* event) override {
		if (event->buttons() & Qt::LeftButton)
			// Set drag mode when left button is pressed
			setDragMode(QGraphicsView::ScrollHandDrag);
		QGraphicsView::mousePressEvent(event);
	}
	void mouseReleaseEvent(QMouseEvent* event) override {
		if (dragMode() & QGraphicsView::ScrollHandDrag)
			// Unset drag mode when left button is released
			setDragMode(QGraphicsView::NoDrag);
		QGraphicsView::mouseReleaseEvent(event);
	}
	void autocomputeSceneSize(const QList<QRectF>& region) {
		Q_UNUSED(region);
		if ( !m_draw ) 
			return;

		//qDebug( ) << region;

		// Widget viewport recangle
		QRectF widget_rect_in_scene(
				mapToScene( m_viewportTopleft )
				, mapToScene( rect( ).bottomRight( ) + m_viewportBottomRight )
			);
		// Copy the new size from the old one
		m_topLeft = sceneRect().topLeft();
		m_bottomRight = sceneRect().bottomRight();

		// Check that the scene has a bigger limit in the top side
		if (sceneRect().top() > widget_rect_in_scene.top())
			m_topLeft.setY(widget_rect_in_scene.top());

		// Check that the scene has a bigger limit in the bottom side
		if (sceneRect().bottom() < widget_rect_in_scene.bottom())
			m_bottomRight.setY(widget_rect_in_scene.bottom());

		// Check that the scene has a bigger limit in the left side
		if (sceneRect().left() > widget_rect_in_scene.left())
			m_topLeft.setX(widget_rect_in_scene.left());

		// Check that the scene has a bigger limit in the right side
		if (sceneRect().right() < widget_rect_in_scene.right())
			m_bottomRight.setX(widget_rect_in_scene.right());

		// Set new scene size
		setSceneRect(QRectF(m_topLeft, m_bottomRight));
		showStatus( );
	}

	std::vector< QGraphicsPolygonItem * > m_vehiclesItems;
	std::vector< QGraphicsPolygonItem * > m_trafficSignalItems;
	std::unique_ptr< Simulation::Draw > m_draw;

	Simulation::Road::roads_t m_roads;

	static const uint c_vehicleRate = 35;
	std::unique_ptr< Simulation::VehicleGenerator > m_vehicleGenerator;

	std::vector< std::unique_ptr< Simulation::Vehicle > > m_vehiclesObjects; // tmp
	std::vector< Simulation::Vehicle > m_vehiclesObjects2;
	uint n_vehicles_generated = 0;
	uint n_vehicles_on_map = 0;

	std::unique_ptr< Simulation::Road::TrafficSignal > m_trafficSignal;
	std::chrono::seconds m_timeToReswitch{ 5 };
	typedef std::chrono::steady_clock time_point_t;
	time_point_t::time_point m_dtSwitchLast;
	const time_point_t::time_point c_dtZero;

	timer_t m_t = 0;
	double m_speed = 3;
	timer_t m_tSpeeded = 0;

	void draw_vehicles() {
#pragma region clear previous
		auto it = std::remove_if( m_vehiclesItems.begin( ), m_vehiclesItems.end( )
				, [this](QGraphicsPolygonItem *p) {
					return scene( ) ->removeItem( p ), delete p, true;
				}
			);
		m_vehiclesItems.erase( it, m_vehiclesItems.end( ) );
#pragma endregion
		QColor colorBlue = QColor::fromRgb( 0, 0, 255 );
		QBrush brush( colorBlue, Qt::SolidPattern );
		QPen pen( colorBlue );
		for ( auto & road : m_roads ) {
			Simulation::vehicles_t const& vehicles = road.getVehicles( );
			for ( auto & vehicle : vehicles ) {
				qreal l = vehicle ->length( ), h = vehicle ->width( );
				qreal sin = road.angle_sin( ), cos = road.angle_cos( );
				qreal x = road.start( ).x( ) + cos * vehicle ->x( );
				qreal y = road.start( ).y( ) + sin * vehicle ->x( );
				auto points = m_draw ->rotated_box( { x, y }, { l, h }, cos, sin, true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				auto vehicleItem = scene( ) ->addPolygon( polygon, pen, brush );
				m_vehiclesItems.push_back( vehicleItem );
			}
		}
	}
	timer_t fixDeltaT() { 
		auto timePoint = now( );
		auto durationFrame = std::chrono::duration_cast
			< std::chrono::microseconds >
			( timePoint - m_dtLast );
		m_dtLast = timePoint;
		auto xxxInSecond = std::chrono::duration_cast
			< std::chrono::microseconds >
			( std::chrono::seconds{ 1 } ).count( );
		return static_cast< timer_t >( durationFrame.count( ) ) / xxxInSecond;
	}

	void animation() {
		//qDebug( ) << "animation";
		auto scene = this ->scene( );
		// ?QGraphicsScene::advance()

		if ( !m_draw ) {
			int zoom = 5;
			m_draw = std::make_unique< Simulation::Draw >( width( ), height( ), zoom );

			// Elements
			QColor colorGrey = QColor::fromRgb( 180, 180, 220 );
			QColor colorRed = QColor::fromRgb( 255, 0, 0 );
			QBrush brush_( colorGrey, Qt::SolidPattern );
			QPen pen_( brush_, 1 );
			//pen_.setColor( colorRed );
			auto polygons = Simulation::AllRoads::calc( width( ), height( ), zoom );
			for ( auto const& polygon : polygons )
				scene ->addPolygon( polygon, pen_, brush_ );
			m_roads = Simulation::AllRoads::get( );

			// tmp, aka vehicle generator
			//auto W_R_S = Simulation::AllRoads::W_R_S( );
			//std::vector< uint > path;
			////path.push_back( 0 );
			////path.insert( path.end( ), W_R_S.begin( ), W_R_S.end( ) );
			////path.push_back( 5 );
			//path.push_back( 2 );
			//path.push_back( 10 );
			//path.push_back( 4 );
			//m_vehiclesObjects.push_back( std::make_unique< Simulation::Vehicle >( path ) );
			//auto &vehicle = m_vehiclesObjects[ 0 ];
			//int firstRoad = 0;
			//auto roadIndex = vehicle ->path( )[ firstRoad ];
			//m_roads[ roadIndex ].addVehicle( vehicle.get( ) );

			auto allPaths = Simulation::AllRoads::getAllPaths( );
			// add_generator
			Simulation::AllRoads::inboundRoads_t inboundRoads;
			// @from https://www.codeconvert.ai/python-to-c++-converter
			for ( auto &path : allPaths ) {
				uint weight = path.first;
				uint road_index = path.second[ 0 ];
				inboundRoads.insert( { road_index, &m_roads[ road_index ] } );
			}

			m_vehicleGenerator = std::make_unique< Simulation::VehicleGenerator >( 
				c_vehicleRate, allPaths, inboundRoads );

			Simulation::Road::TrafficSignal::signalRoads_t signalRoads;
			Simulation::AllRoads::signalIdxRoads_t signalIdxRoads = 
				Simulation::AllRoads::getSignalIdxRoads( );
			for ( auto const& pair : signalIdxRoads ) 
				signalRoads.push_back( { &m_roads[ pair[ 0 ] ], &m_roads[ pair[ 1 ] ] } );
			m_trafficSignal = std::make_unique< Simulation::Road::TrafficSignal >( 
				signalRoads );
		}

		timer_t dt;
		// OR measured
		dt = fixDeltaT( );
		// OR fixed for smooth moving
//		m_dtLast = now( ), dt = 0.08;
		timer_t dtSpeeded = dt * m_speed;
		m_tSpeeded += dtSpeeded;
		timer_t t = m_tSpeeded;

		draw_vehicles( );

		//# Update every road // self.roads[i].update(self.dtSpeeded, self.t)
		{
		for ( auto & road : m_roads ) {
			Simulation::vehicles_t vehicles_ = road.getVehicles( );
			if ( vehicles_.empty( ) )
				continue;
			std::vector< Simulation::IVehicle * > vehicles( 
				vehicles_.begin( ), vehicles_. end( ) );
			Simulation::IVehicle* lead = vehicles.front( );

			if ( road.traffic_signal_state( ) ) {
				//# If traffic signal is green (or doesn't exist), let vehicles pass
				lead ->unstop( t );
				for ( auto & vehicle : vehicles ) 
					vehicle ->unslow( );
			} else {
				if ( road.hasTrafficSignal( ) ) {
					//# The traffic signal is red (existence checked to access its stop_distance)
					bool lead_can_stop_safely = false;
					if ( lead ->x( ) <= ( road.length( ) - m_trafficSignal ->c_stopDistance / 1.5 ) )
						lead_can_stop_safely = true;
					if ( lead_can_stop_safely ) {
						//# slow vehicles in slow zone
						lead ->slow( m_trafficSignal ->c_slowFactor );
						bool lead_in_stop_zone = false;
						if ( ( road.length( ) - m_trafficSignal ->c_stopDistance ) <= lead ->x( ) )
							lead_in_stop_zone = true;
						if ( lead_in_stop_zone )
							lead ->stop( t );
					}
				}
			}

			//# Update first vehicle
			lead ->update( nullptr, dtSpeeded, &road );
			//# Update other vehicles
			for ( uint i = 1; i < vehicles.size( ); ++i ) { 
				lead = vehicles[ i - 1 ];
				vehicles[ i ] ->update( lead, dtSpeeded, &road );
			}
		}
		}

		auto road_index = m_vehicleGenerator ->update( t, &n_vehicles_generated );
		if ( road_index ) {
			++n_vehicles_generated;
			++n_vehicles_on_map;
			// TODO(alex): makeme
			//self._non_empty_roads.add(road_index)
		}

		// self._check_out_of_bounds_vehicles()
		for ( auto & road : m_roads ) {
			Simulation::vehicles_t const& vehicles = road.getVehicles( );
			if ( vehicles.empty( ) ) 
				continue;
			Simulation::IVehicle* lead = vehicles[ 0 ];
			//# If first vehicle is out of road bounds
			if ( lead ->x( ) < road.length( ) ) 
				continue;
			//# If vehicle has a next road
			auto nextRoadIndex = ( lead ->currentRoadIndex( ) + 1 );
			auto &path = lead ->path( );
			if ( nextRoadIndex < path.size( ) ) {
				// Remove it from its road
				road.popFrontVehicle( );
				// Reset the position relative to the road
				lead ->resetPositionOnRoad( );
				// Add it to the next road
				lead ->incCurrentRoadIndex( );
				auto next_road_index = path[ nextRoadIndex ];
				// TODO(alex): makeme
				//new_non_empty_roads.add(next_road_index);
				m_roads[ next_road_index ].addVehicle( lead );
			} else {
				//# Remove it from its road
				road.popFrontVehicle( );
				// TODO(alex): makeme
				//# Remove from non_empty_roads if it has no vehicles
				//if not road.vehicles:
				//    new_empty_roads.add(road.index)
				--n_vehicles_on_map;
				//# Update the waiting times sum
				//self._waiting_times_sum += lead.get_wait_time(self.t)
			}
		}

		{
#pragma region clear previous
		{
			auto it = std::remove_if( m_trafficSignalItems.begin( ), m_trafficSignalItems.end( )
					, [&scene](QGraphicsPolygonItem *p) {
						return scene ->removeItem( p ), delete p, true;
					}
				);
			m_trafficSignalItems.erase( it, m_trafficSignalItems.end( ) );
		}
#pragma endregion
		Simulation::Road::TrafficSignal::signalRoads_t signalRoads = 
			m_trafficSignal ->getRoads( );
		for ( uint i = 0; i < signalRoads.size( ); ++i ) {
			QColor color;
			QColor red = QColor::fromRgb( 255, 0, 0 );
			QColor green = QColor::fromRgb( 0, 255, 0 );
			auto currentCycle = m_trafficSignal ->getCurrentCycle( );
			const std::array<bool, 2> stat{ false, false };
			bool b = ( currentCycle == stat );
			if ( b ) {
				QColor yellow = QColor::fromRgb( 255, 255, 0 );
				uint currentCycleIndex = m_trafficSignal ->currentCycleIndex( );
				auto cycle = m_trafficSignal ->cycle( );
				if ( cycle[ currentCycleIndex - 1 ][ i ] )
					color = yellow;
				else 
					color = red;
			} else {
				if ( currentCycle[ i ] )
					color = green;
				else
					color = red;
			}
            for ( auto &road : m_trafficSignal ->getRoads( )[i] ) {
                qreal a = 0;
                QPointF position {
						(1 - a) * road ->end( ).x( ) + a * road ->start( ).x( )
						, (1 - a) * road ->end( ).y( ) + a * road ->start( ).y( )
					};
				auto points = m_draw ->rotated_box( position, { 1, 3 },
                                    road ->angle_cos( ), road ->angle_sin( ), true );
				QPolygonF polygon;
				for ( auto const& elem : points )
					polygon << elem;
				QPen pen( color );
				QBrush brush( color, Qt::SolidPattern );
				auto trafficSignalItem = scene ->addPolygon( polygon, pen, brush );
				m_trafficSignalItems.push_back( trafficSignalItem );
			}
		}
		}

		// _update_signals
		if ( c_dtZero == m_dtSwitchLast ) {
			m_dtSwitchLast = now( );
		} else {
			if ( m_dtLast > ( m_dtSwitchLast + m_timeToReswitch ) ) {
				m_dtSwitchLast = now( );
				m_trafficSignal ->update( );
			}
		}

        //# Increment time
		m_t += dt;
	}

public: 
	int m_delta = 5;
	static constexpr auto now = time_point_t::now;
	time_point_t::time_point m_dtLast;
    MyQGraphicsView(QMainWindow *parent) : 
		QGraphicsView( parent )
		, m_parent( parent )
	{
		setScene( new QGraphicsScene( this ) );

		setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		auto connection0 = connect( scene(), &QGraphicsScene::changed, this, &MyQGraphicsView::autocomputeSceneSize);
		//auto connection1 = connect( scene(), &QGraphicsScene::sceneRectChanged, this, &MyQGraphicsView::autocomputeSceneSize2);

		m_dtLast = now( );
		// @insp https://stackoverflow.com/questions/28728820/qgraphicsview-doesnt-always-update
		QObject::connect( &m_timer, &QTimer::timeout
				, [this] { 
					this ->animation( );
				}
			);
		m_timer.start( 50 );
		// QGraphicsView::ViewportUpdateMode, default is QGraphicsView::MinimalViewportUpdate

		setRenderHints( QPainter::Antialiasing );
	}
    MyQGraphicsView(const MyQGraphicsView&) = delete;
    MyQGraphicsView& operator=(const MyQGraphicsView&) = delete;
};
W_OBJECT_IMPL( MyQGraphicsView ) //Q_OBJECT

#include "uic/ui_mainwindow.h"
class MainWindow : public QMainWindow {
	W_OBJECT( MainWindow ) //Q_OBJECT
	Ui::MainWindow *ui = nullptr;

public:
    explicit MainWindow(QWidget *p = 0) : 
		QMainWindow( p )
		, ui( new Ui::MainWindow )
	{
		// Got from QtCreator
		ui ->setupUi( this );
	}
	~MainWindow() {
		delete ui;
	}
};
W_OBJECT_IMPL( MainWindow ) //Q_OBJECT

struct viaQGraphicsView { static void run(int argc, char* argv[]) { 
		//QLoggingCategory::setFilterRules( "qt.scenegraph.general=true" );
		QApplication app( argc, argv );
		MainWindow mainWindow;
		mainWindow.show( );
		app.exec( );
	}
};
