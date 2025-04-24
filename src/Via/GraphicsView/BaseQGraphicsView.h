#pragma once // src\Via\GraphicsView\BaseQGraphicsView.h - something based
#include "Common.h"
namespace syscross::TraffModel::Via::GraphicsView {
class BaseQGraphicsView : public QGraphicsView, public Common {
public: 
    explicit BaseQGraphicsView(QMainWindow *parent) : 
		QGraphicsView( parent )
	{
		setScene( new QGraphicsScene( this ) );
		setRenderHints( QPainter::Antialiasing );
	}
    BaseQGraphicsView(const BaseQGraphicsView&) = delete;
    BaseQGraphicsView& operator=(const BaseQGraphicsView&) = delete;
};
} // namespace syscross::TraffModel::Via::GraphicsView
