#pragma once // src\Via\GraphicsView\BaseQGraphicsView.h - something based
#include "Common.h"
namespace syscross::TraffModel::Via::GraphicsView {
/**
 * Base QGraphicsView with common initialization
 */
class BaseQGraphicsView : public QGraphicsView, public Common {
public:
	/**
	 * @brief Constructs the base QGraphicsView
	 * @param parent QGraphicsView
	 */
	explicit BaseQGraphicsView(QMainWindow *parent) :
		QGraphicsView( parent )
	{
		setScene( new QGraphicsScene( this ) );
		setRenderHints( QPainter::Antialiasing );
	}

	// Disable copying and moving
	BaseQGraphicsView(const BaseQGraphicsView&) = delete;
	BaseQGraphicsView& operator=(const BaseQGraphicsView&) = delete;
};
} // namespace syscross::TraffModel::Via::GraphicsView
