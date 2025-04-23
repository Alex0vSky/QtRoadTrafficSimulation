// stdafx.h - pch

#ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmicrosoft-include"
#	pragma clang diagnostic ignored "-Wmicrosoft-cast"
//-fmacro-backtrace-limit=0
#endif // __clang__

// ...

#ifdef __clang__
#	pragma clang diagnostic pop
#endif // __clang__


// allow rand_s
#define _CRT_RAND_S
// disable min and max macros
#define NOMINMAX
#if defined(DEBUG) || defined(_DEBUG)
#	include <crtdbg.h>
#endif
#pragma warning( push )
#pragma warning( disable: 5039 )

// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <stdexcept>
#include <iostream>

#include <atomic>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <intrin.h>

#include <deque>
#include <set>
#include <sstream>
#include <optional>
#include <array>

#include <iomanip>
#include <conio.h>


// Qt stuff
	// for examples doc.qt.io/qt-6/qtwidgets-painting-basicdrawing-example.html
#define QT_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(5, 15, 0)
////#include <QtCore/QtCore>
////#include <QtGui/QtGui>
//#include <QtWidgets/qapplication.h>
//#include <QtWidgets/qdialog.h>
//#include <QtWidgets/qlabel.h>
#include <QtWidgets/QtWidgets>

#include <QtCore/QObject>
#include <QtWidgets/QGraphicsView>

#include <QtQuick/QtQuick>
#include <QtQuickWidgets/QQuickWidget>

//#include <QtQml/QQmlEngine>
//#include <QtQuick/QQuickView>
//#include "QtQuick/qsgengine.h"
////#include <QtDeclarative/QDeclarativeView>
////#include <QtDeclarative/QDeclarativeItem>

#ifndef A0S_QT_PRO
// For header-only in woboq/verdigris
#	include <wobjectdefs.h>
#	include <wobjectimpl.h>
#else
#	define W_OBJECT(none) Q_OBJECT
#	define W_OBJECT_IMPL(none)
#endif // A0S_QT_PRO

#pragma warning( pop )
