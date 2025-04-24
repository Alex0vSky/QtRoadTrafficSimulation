// stdafx.h - pch // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#ifndef STABLE_H
#define STABLE_H

// Qt stuff
#ifndef A0S_QT_PRO
// for examples doc.qt.io/qt-6/qtwidgets-painting-basicdrawing-example.html
#	define QT_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(5, 15, 0)
// For header-only in woboq/verdigris
#	include <wobjectdefs.h>
#	include <wobjectimpl.h>
#else // A0S_QT_PRO
#	define W_OBJECT(none)
#	define W_OBJECT_IMPL(none)
#endif // A0S_QT_PRO


/* Add C includes here */
#if defined( _WIN32 ) && defined( _DEBUG ) 
#	include <windows.h>
#	include <crtdbg.h>
#endif // #if defined( _WIN32 ) && defined( _DEBUG ) 


#if defined __cplusplus
/* Add C++ includes here */

// C RunTime Header Files
#include <sstream>
#include <vector>
#include <array>
#include <deque>
#include <chrono>
#include <optional>

#include <QtWidgets/QtWidgets>

#include <QtCore/QObject>
#include <QtWidgets/QGraphicsView>

#include <QtQuick/QtQuick>
#include <QtQuickWidgets/QQuickWidget>

#endif // __cplusplus

#endif // STABLE_H
