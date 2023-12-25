﻿// src\Via\GraphicsView\BaseQGraphicsView.h - render via QGraphicsView, render in main thread
namespace syscross::QtRoadTrafficSimulation::Via::GraphicsView {
class BaseQGraphicsView : public QGraphicsView {
	W_OBJECT( BaseQGraphicsView ) //Q_OBJECT

protected:
	QMainWindow *m_parent;

public: 
    BaseQGraphicsView(QMainWindow *parent) : 
		QGraphicsView( parent )
		, m_parent( parent )
	{
		setScene( new QGraphicsScene( this ) );
		setRenderHints( QPainter::Antialiasing );
	}
    BaseQGraphicsView(const BaseQGraphicsView&) = delete;
    BaseQGraphicsView& operator=(const BaseQGraphicsView&) = delete;
};
W_OBJECT_IMPL( BaseQGraphicsView ) //Q_OBJECT
} // namespace syscross::QtRoadTrafficSimulation::Via::GraphicsView
