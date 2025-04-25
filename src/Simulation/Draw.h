#pragma once // src\Simulation\Draw.h - drawing tools
namespace syscross::TraffModel::Sim {
/**
 * Rendering utilities for simulation objects
 */
class Draw {
	/// Viewport width in pixels
	const uint c_width = 1000;

	/// Viewport height in pixels
	const uint c_height = 630;

	/// Zoom scaling factor
	const uint c_zoom = 5;

	/// Screen offset adjustment
	const QPoint c_offsetScreen{0, 0};

	/**
	 * Converts simulation coordinates to screen space
	 *
	 * Applies transformations:
	 * - Viewport centering
	 * - Zoom scaling
	 * - Manual offset adjustment
	 *
	 * @param point Simulation coordinates to convert
	 * @return Corresponding screen coordinates
	 */
	QPointF convert_(QPointF const& point) const {
		return QPointF(
				c_width / 2 + ( point.x( ) + c_offsetScreen.x( ) ) * c_zoom
				, c_height / 2 + ( point.y( ) + c_offsetScreen.y( ) ) * c_zoom
			);
	}

public:
	/**
	 * @brief Generates rotated box polygon
	 *
	 * Calculates vertex positions for:
	 * - Centered or edge-aligned boxes
	 * - Arbitrary rotation angles
	 * - Screen-space conversion
	 *
	 * @param pos Center position
	 * @param size Box dimensions
	 * @param cos Rotation cosine
	 * @param sin Rotation sine
	 * @param centered Placement mode
	 * @return Transformed polygon points
	 */
	auto rotated_box(QPointF pos, QPointF size, qreal cos, qreal sin, bool centered) {
        const qreal x = pos.x( ), y = pos.y( );
        const qreal l = size.x( ), h = size.y( );
		// Vertex calculation lambda with rotation transform
		auto lambdaVertex = [&](qreal e1, qreal e2) {
				// Apply rotation matrix to local coordinates:
				// x' = x*cos - y*sin
				// y' = x*sin + y*cos
				// Then offset by position and convert to screen space
				return QPointF(
						x + (e1 * l * cos + e2 * h * sin) / 2
						, y + (e1 * l * sin - e2 * h * cos) / 2
					);
			};
		std::vector< QPointF > points( 4 );
		if ( centered ) {
	        // Centered rectangle mode - symmetric around position:
			// Bottom-left
			points[ 0 ] = convert_( lambdaVertex( -1, -1 ) );
			// Top-left
			points[ 1 ] = convert_( lambdaVertex( -1, +1 ) );
			// Top-right
			points[ 2 ] = convert_( lambdaVertex( +1, +1 ) );
			// Bottom-right
			points[ 3 ] = convert_( lambdaVertex( +1, -1 ) );
		} else {
	        // Edge-aligned mode - extends from position:
			// Start of bottom edge
			points[ 0 ] = convert_( lambdaVertex( +0, -1 ) );
			// Start of top edge
			points[ 1 ] = convert_( lambdaVertex( +0, +1 ) );
			// End of top edge (2x length)
			points[ 2 ] = convert_( lambdaVertex( +2, +1 ) );
			// End of bottom edge
			points[ 3 ] = convert_( lambdaVertex( +2, -1 ) );
		}
		return points;
	}
};
} // namespace syscross::TraffModel::Sim
