// src\QrcLocatorReplacement.h - indeed, there are *.qml-s in ./resource/qml
namespace syscross::TraffModel {
class QrcLocatorReplacement { 
	class Interceptor : public QQmlAbstractUrlInterceptor {
		QQmlAbstractUrlInterceptor *m_parent;
		QString m_prefix;
		QUrl intercept(const QUrl& path, QQmlAbstractUrlInterceptor::DataType type) override {
			do {
				if ( QQmlAbstractUrlInterceptor::QmlFile != type )
					break;
				if ( path.scheme() != "qrc" )
					break;
				QFileInfo fileInfo( m_prefix + ":" + path.path( ) );
				if ( !fileInfo.exists( ) )
					break;
				return QUrl::fromLocalFile( fileInfo.filePath( ) );
			} while ( false );
			if ( !m_parent )
				return path;
			return m_parent ->intercept( path, type );
		};
	public:
		Interceptor(QQmlAbstractUrlInterceptor *parent, QString prefix) :
			m_parent( parent )
			, m_prefix( prefix )
		{}
	};

	std::unique_ptr< Interceptor > m_urlInterceptor;
	QQmlAbstractUrlInterceptor* m_interceptorPrev = nullptr;
	QQmlApplicationEngine *m_engine;

public: 
	QrcLocatorReplacement(QQmlApplicationEngine *engine) : 
		m_engine( engine )
	{
		auto currentSystemPath = QDir::currentPath( );
		auto currentBinaryPath = qApp ->applicationDirPath( );
		QStringList searchPaths{ 
				currentSystemPath + "/../resource/qml"
				, currentBinaryPath + "/../../../resource/qml"
				, currentBinaryPath + "/../resource/qml"
			};
		QString prefix = "qrc";
		QDir::setSearchPaths( prefix, searchPaths );
		// engine.setBaseUrl not working or use Q_INIT_RESOURCE
		// @insp https://www.kdab.com/fun-with-paths-urls-in-qml/
		// @insp https://stackoverflow.com/questions/39701903/difference-between-foo-qrc-foo-and-qrc-foo-paths-in-qt
#ifndef A0S_QT_PRO
		m_interceptorPrev = m_engine ->urlInterceptor( );
#endif // A0S_QT_PRO
		m_urlInterceptor = std::make_unique< Interceptor > ( m_interceptorPrev, prefix );
		m_engine ->setUrlInterceptor( m_urlInterceptor.get( ) );
	}
	~QrcLocatorReplacement() {
		m_engine ->setUrlInterceptor( m_interceptorPrev );
	}
};
} // namespace syscross::TraffModel
