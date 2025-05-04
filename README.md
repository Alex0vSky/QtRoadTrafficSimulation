# QtRoadTrafficSimulation - Road Traffic Simulator with Qt
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://en.wikipedia.org/wiki/C++)
[![Qt](https://img.shields.io/badge/Qt-5.15-blue?logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-green)](https://github.com/Alex0vSky/QtRoadTrafficSimulation/blob/main/LICENSE)

![Demo](https://github.com/Alex0vSky/QtRoadTrafficSimulation/assets/52796897/f26689eb-eb79-459b-86b8-198bb3f7aa7c)

A road traffic simulator with three different Qt-based visualization approaches.
Cars and roads based on __python__ solution from *BilHim/trafficSimulator* and *yossidoctor/AI-Traffic-Lights-Controller*

## ✨ Key Features

### 🏗️ Architecture
- **Flexible rendering system** with three independent implementations
- **Separation of logic and presentation** through _Common_, _Simulation_ and _Via_ components
- **Simulation state** managed via _State_ pattern

### 🚦 Simulation Model
- **Road network** with straight sections and turns - _AllRoads_
- **Traffic light control** with fixed cycles - _TrafficSignal_
- **Vehicle generation** with different routes - _VehicleGenerator_
- **Realistic movement physics** _Vehicle_ including:
  - Acceleration/braking
  - Distance keeping
  - Traffic light response

### 🎨 Visualization
Three implementation approaches with common functionality:
1. **QGraphicsView**:
   - Classic approach with main-thread rendering
   - Uses _QGraphicsPolygonItem_ for drawing
   - Simpler implementation but less performant

2. **QQuickItem**:
   - Modern Scene Graph rendering
   - Separate render thread (high performance)
   - Uses _QSGGeometryNode_ for efficient drawing

3. **QQuickPaintedItem**:
   - QPainter-based rendering in separate thread
   - More flexible but less performant than _QQuickItem_
   - Uses standard Qt painting methods

### ⚙️ Technical Highlights
- **Alternative QML resource locator** - _QrcLocatorReplacement_:
  - Enables QML usage without Qt plugins in MSVC
  - Searches for QML files in multiple paths
  - Uses _QQmlAbstractUrlInterceptor_ for qrc path redirection

- **MOC-free operation support**:
  - Conditional compilation with _A0S_QT_PRO_
  - Alternative _Q_OBJECT_ implementation via _W_OBJECT/W_OBJECT_IMPL_
  - Compilation without Qt's meta-object system

- **Unified polygon handling**:
  - Consistent _QPolygonF_ usage across all implementations
  - Qt6 compatibility adaptations:
    - Replaced deprecated _DrawTriangleFan_ with _DrawTriangleStrip_
    - Replaced _DrawLineLoop_ with _DrawLineStrip_
  - Benefits:
    - Compatibility with both Qt5 and Qt6
    - More modern and efficient rendering methods
    - Improved graphics performance

## 📦 Installation & Setup

### Build Options
The project supports two build methods:

1. **Standard build** - `HelloQt.pro`:
   - For Qt Creator and other IDEs
   - Uses standard Qt build system

2. **MOC-free build** - `HelloQt(Non_MocUic).sln`:
   - For Visual Studio 2019
   - Specifically configured for Qt 5.15
   - Without Qt 5.15 you may encounter errors like:
     ```
     The code execution cannot proceed because Qt5Cored.dll was not found
     The code execution cannot proceed because Qt5Guid.dll was not found
     The code execution cannot proceed because Qt5Quickd.dll was not found
     The code execution cannot proceed because Qt5Widgetsd.dll was not found
     ```

### Requirements

#### For Visual Studio 2019 Build
- Qt 5.15 (exact version required)
- Microsoft Visual Studio 2019 Community
#### For Qt Creator/Other IDEs (HelloQt.pro)
- Qt 5.15 or later
- Standard Qt build system
- Any IDE supporting .pro files (Qt Creator, CLion, etc.)

## 🚀 Usage
- Launch the application
- Use mouse wheel to zoom
- Drag the scene with left mouse button
- Observe vehicle movement and traffic light operation

## 🤝 Contributing
PRs and questions are welcome! No strict requirements for contributors.

## 📚 Acknowledgments
- [Qt](https://www.qt.io/) - for the framework
- [BilHim/trafficSimulator](https://github.com/BilHim/trafficSimulator) - for the model basis
- [yossidoctor/AI-Traffic-Lights-Controller](https://github.com/yossidoctor/AI-Traffic-Lights-Controller) - for traffic light ideas
- [woboq/verdigris](https://github.com/woboq/verdigris) - for MOC-free operation

## 📜 License
Project is MIT licensed. See [LICENSE](https://github.com/Alex0vSky/QtRoadTrafficSimulation/blob/main/LICENSE) for details.
