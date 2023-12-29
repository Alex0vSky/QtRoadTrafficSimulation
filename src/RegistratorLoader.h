// src\RegistratorLoader.h - register Qml type, load url and checking via connect to engine
namespace syscross::TraffModel {
template<typename T>
class RegistratorLoader { 
public: 
	RegistratorLoader(QQmlApplicationEngine *engine, QGuiApplication *app, const char *name, QString url_) {
		qmlRegisterType< T >( name, 1, 0, name );
		QrcLocatorReplacement replacement( engine );
		QUrl url( url_ );
		QObject::connect(
				engine
				, &QQmlApplicationEngine::objectCreated
				, app
				, [url](QObject *obj, QUrl const& objUrl) {
					if ( !obj && url == objUrl )
						QCoreApplication::exit( -1 );
				}
				, Qt::QueuedConnection
			);
		engine ->load( url );
		if ( engine ->rootObjects( ).isEmpty( ) )
			QCoreApplication::exit( -1 );
	}
};
} // namespace syscross::TraffModel
