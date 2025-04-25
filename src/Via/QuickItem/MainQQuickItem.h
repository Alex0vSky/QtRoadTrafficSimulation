// src\Via\QuickItem\MainQQuickItem.h - main loop in separate thread
#include "Via/QuickItem/BaseQQuickItem.h"
#include "Via/QuickItem/ZoomableQQuickItem.h"
#include "Via/QuickItem/DraggableQQuickItem.h"
namespace syscross::TraffModel::Via::QuickItem {
/**
 * QQuickItem implementation for traffic simulation rendering
 */
class MainQQuickItem : public DraggableQQuickItem {
	/// Root node for vehicle rendering
	QSGNode *m_carsNode = nullptr;

	/// Root node for traffic light rendering
	QSGNode *m_ligthsNode = nullptr;

	/// Root node for road network rendering
	QSGNode *m_roadsNode = nullptr;

	/// FPS display text object
	QObject *m_textFps = nullptr;

	/// FPS counter utility
	FpsCounter m_fps;

	/**
	 * @brief Creates polygon geometry node
	 *
	 * Builds:
	 * - Vertex geometry with UV mapping
	 * - Material with solid color
	 * - Node hierarchy ownership
	 *
	 * @param node Parent node for attachment
	 * @param polygon Shape vertices
	 * @param mode OpenGL drawing mode
	 * @param color Fill color
	 */
	void addPolygon_(QSGNode* node, QPolygonF const& polygon, uint mode, QColor color) {
		const auto first = polygon.at( 0 );
	    // Create geometry with Point2D attributes
		QSGGeometry *geometry = new QSGGeometry( QSGGeometry::defaultAttributes_Point2D( ), polygon.size( ) + 1 );
	    // Configure drawing parameters
		geometry ->setDrawingMode( mode );
		geometry ->setLineWidth( 1 );
	    // Fill vertex buffer with polygon points
		QSGGeometry::Point2D *vertices = geometry ->vertexDataAsPoint2D( );
		for ( auto const& point : polygon )
			(vertices++) ->set( point.x( ), point.y( ) );
	    // Close polygon by repeating first point
		(vertices++) ->set( first.x( ), first.y( ) );
	    // Create geometry node
		QSGGeometryNode *n = new QSGGeometryNode;
		n ->setGeometry( geometry );
		n ->setFlag( QSGNode::OwnsGeometry);
		// Configure solid color material
		QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
		material ->setColor( color );
		n ->setMaterial( material );
		n ->setFlag( QSGNode::OwnsMaterial);
		// Attach to parent node
		node ->appendChildNode( n );
	}

	/**
	 * @brief Updates scene graph nodes
	 *
	 * Handles:
	 * - Initial resource creation
	 * - Vehicle position updates
	 * - Traffic light state changes
	 * - FPS counter updates
	 *
	 * @param oldNode Previous frame's root node
	 * @param data Node update metadata
	 * @return Updated scene graph node
	 */
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override {
		// Initialize new scene graph if none exists
		if ( !oldNode ) {
			oldNode = new QSGNode;
			// Create static road network geometry
			oldNode ->appendChildNode( m_roadsNode = new QSGNode );
			auto polygons = Sim::AllRoads::calc( );
			for ( QPolygonF const& polygon : polygons ) {
				// lower
				addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawTriangleStrip, Qt::gray );
				// TODO(alex): write comment
				//// upper
				//addPolygon_( m_roadsNode, polygon, QSGGeometry::DrawLineStrip, Qt::red );
			}
			Common::init( );
			// Initialize dynamic elements containers
			oldNode ->appendChildNode( m_carsNode = new QSGNode );
			oldNode ->appendChildNode( m_ligthsNode = new QSGNode );
			// Locate FPS display item in QML
			m_fps.reset( );
		}

		// Process simulation timing
		auto measurerScoped = m_timing.createAutoMeasurerScoped( );
		auto [ t, dt ] = measurerScoped.get( );

		// Update vehicles visualization
		while ( QSGNode* node = m_carsNode ->firstChild( ) )
			delete node;
		m_scener ->drawVehicles( [this](QPolygonF const& polygons) {
				addPolygon_( m_carsNode, polygons, QSGGeometry::DrawTriangleStrip, Qt::blue );
				return nullptr;
			} );
		m_updater ->roads( t, dt );
		// Handle vehicle spawning/removal
		auto road_index = m_vehicleGenerator ->update( t );
		if ( road_index ) {
			++m_vehiclesOnMap;
		}
		m_updater ->outOfBoundsVehicles( &m_vehiclesOnMap );

		// Update traffic lights visualization
		while ( QSGNode* node = m_ligthsNode ->firstChild( ) )
			delete node;
		m_scener ->drawSignals( [this](QPolygonF const& polygons, QColor color) {
				addPolygon_( m_ligthsNode, polygons, QSGGeometry::DrawTriangleStrip, color );
				return nullptr;
			} );
		m_updater ->trafficSignals( t );

		// Apply view transformations. Empty matrix, or wrong zoom will be soon
		QMatrix4x4 m_matrix;
		DraggableQQuickItem::handleDrag( &m_matrix );
		ZoomableQQuickItem::handleZoom( &m_matrix );
		data ->transformNode ->setMatrix( m_matrix );

		// Update FPS counter display
		if ( auto fps = m_fps.incrementFrame( ) )
			m_textFps ->setProperty( "text", fps ->c_str( ) );

		// Schedule next frame, smooth animation
		update( );
		return oldNode;
	}

protected:
    void componentComplete() override {
        QQuickItem::componentComplete( );
        m_textFps = findChild< QObject *>( "FPS" );
        Q_ASSERT_X( m_textFps, "Init", "FPS label must exist in QML" );
    }
};
} // namespace syscross::TraffModel::Via::QuickItem
