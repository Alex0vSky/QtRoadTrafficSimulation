// src\Via\GraphicsView\MainWindow.h - main window
namespace syscross::TraffModel::Via::GraphicsView {
#include "uic/ui_mainwindow.h"
class MainWindow : public QMainWindow {
	W_OBJECT( MainWindow ) //Q_OBJECT
	Ui::MainWindow *ui = nullptr;

public:
    explicit MainWindow(QWidget *p = nullptr) : 
		QMainWindow( p )
		, ui( new Ui::MainWindow )
	{
		// Got from QtCreator/uic
		ui ->setupUi( this );
	}
	~MainWindow() {
		delete ui;
	}
};
W_OBJECT_IMPL( MainWindow ) //Q_OBJECT
} // namespace syscross::TraffModel::Via::GraphicsView
