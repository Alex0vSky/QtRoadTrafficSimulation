// src\Info.h - just do it // @insp https://github.com/huangqinjin/CMakeQt/blob/master/main.cpp
struct Info {
	static void to_qDebug() {
#if QT_VERSION_MAJOR < 6
#	define path location
#endif
		qDebug( ) << "QLibraryInfo::PrefixPath            " << QLibraryInfo::path( QLibraryInfo::PrefixPath );
		qDebug( ) << "QLibraryInfo::DocumentationPath	 " << QLibraryInfo::path( QLibraryInfo::DocumentationPath );
		qDebug( ) << "QLibraryInfo::HeadersPath	         " << QLibraryInfo::path( QLibraryInfo::HeadersPath );
		qDebug( ) << "QLibraryInfo::LibrariesPath	     " << QLibraryInfo::path( QLibraryInfo::LibrariesPath );
		qDebug( ) << "QLibraryInfo::LibraryExecutablesPath" << QLibraryInfo::path( QLibraryInfo::LibraryExecutablesPath );
		qDebug( ) << "QLibraryInfo::BinariesPath	         " << QLibraryInfo::path( QLibraryInfo::BinariesPath );
		qDebug( ) << "QLibraryInfo::PluginsPath	         " << QLibraryInfo::path( QLibraryInfo::PluginsPath );
#if QT_VERSION_MAJOR < 6
		qDebug( ) << "QLibraryInfo::ImportsPath           " << QLibraryInfo::path( QLibraryInfo::ImportsPath );
		qDebug( ) << "QLibraryInfo::Qml2ImportsPath	     " << QLibraryInfo::path( QLibraryInfo::Qml2ImportsPath );
#else
		qDebug( ) << "QLibraryInfo::QmlImportsPath	     " << QLibraryInfo::path( QLibraryInfo::QmlImportsPath );
#endif
		qDebug( ) << "QLibraryInfo::ArchDataPath          " << QLibraryInfo::path( QLibraryInfo::ArchDataPath );
		qDebug( ) << "QLibraryInfo::DataPath              " << QLibraryInfo::path( QLibraryInfo::DataPath );
		qDebug( ) << "QLibraryInfo::TranslationsPath      " << QLibraryInfo::path( QLibraryInfo::TranslationsPath );
		qDebug( ) << "QLibraryInfo::ExamplesPath	         " << QLibraryInfo::path( QLibraryInfo::ExamplesPath );
		qDebug( ) << "QLibraryInfo::TestsPath             " << QLibraryInfo::path( QLibraryInfo::TestsPath );
		qDebug( ) << "QLibraryInfo::SettingsPath          " << QLibraryInfo::path( QLibraryInfo::SettingsPath );
#if QT_VERSION_MAJOR < 6
#	undef path
#endif
	}
};
