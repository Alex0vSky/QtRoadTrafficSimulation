# QtRoadTrafficSimulation - Road Traffic Simulator with Qt
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://en.wikipedia.org/wiki/C++)
[![Qt](https://img.shields.io/badge/Qt-5.15-blue?logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-green)](https://github.com/Alex0vSky/QtRoadTrafficSimulation/blob/main/LICENSE)

![Demo](https://github.com/Alex0vSky/QtRoadTrafficSimulation/assets/52796897/f26689eb-eb79-459b-86b8-198bb3f7aa7c)

A road traffic simulator with three different Qt-based visualization approaches.
Cars and roads based on __python__ solution from *BilHim/trafficSimulator* and *yossidoctor/AI-Traffic-Lights-Controller*
![QtRoadTrafficSimulation](https://github.com/Alex0vSky/QtRoadTrafficSimulation/assets/52796897/f26689eb-eb79-459b-86b8-198bb3f7aa7c)

## ✨ Key Features

### 🏗️ Architecture
- **Flexible rendering system** with three independent implementations
- **Separation of logic and presentation** through Common, Simulation and Via components
- **Simulation state** managed via State pattern

### 🚦 Simulation Model
- **Road network** with straight sections and turns (AllRoads)
- **Traffic light control** with fixed cycles (TrafficSignal)
- **Vehicle generation** with different routes (VehicleGenerator)
- **Realistic movement physics** (Vehicle) including:
  - Acceleration/braking
  - Distance keeping
  - Traffic light response

### 🎨 Visualization
Three implementation approaches with common functionality:
1. **QGraphicsView**:
   - Classic approach with main-thread rendering
   - Uses QGraphicsPolygonItem for drawing
   - Simpler implementation but less performant

2. **QQuickItem**:
   - Modern Scene Graph rendering
   - Separate render thread (high performance)
   - Uses QSGGeometryNode for efficient drawing

3. **QQuickPaintedItem**:
   - QPainter-based rendering in separate thread
   - More flexible but less performant than QQuickItem
   - Uses standard Qt painting methods

### ⚙️ Technical Highlights
- **Alternative QML resource locator** (QrcLocatorReplacement):
  - Enables QML usage without Qt plugins in MSVC
  - Searches for QML files in multiple paths
  - Uses QQmlAbstractUrlInterceptor for qrc path redirection

- **MOC-free operation support**:
  - Conditional compilation with A0S_QT_PRO
  - Alternative Q_OBJECT implementation via W_OBJECT/W_OBJECT_IMPL
  - Compilation without Qt's meta-object system

- **Unified polygon handling**:
  - Consistent QPolygonF usage across all implementations
  - Qt6 compatibility adaptations:
    - Replaced deprecated DrawTriangleFan with DrawTriangleStrip
    - Replaced DrawLineLoop with DrawLineStrip
  - Benefits:
    - Compatibility with both Qt5 and Qt6
    - More modern and efficient rendering methods
    - Improved graphics performance

## 📦 Installation & Setup

### Build Options
The project supports two build methods:

1. **Standard build** (HelloQt.pro):
   - For Qt Creator and other IDEs
   - Uses standard Qt build system

2. **MOC-free build** (HelloQt(Non_MocUic).sln):
   - For Visual Studio 2019
   - Specifically configured for Qt 5.15
   - Requires additional environment variables:
     ```
     PATH=%PATH%;C:\Prj\_L\Qt\bin;C:\Prj\_L\Qt\qml
     QT_PLUGIN_PATH=C:\Prj\_L\Qt\plugins
     ```
   - Without these settings you may encounter errors like:
     ```
     The code execution cannot proceed because Qt5Cored.dll was not found
     ```

### Requirements
- Qt 5.15 (exact version required for MOC-free build)
- Microsoft Visual Studio 2019 Community (for .sln build)
- C++17

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
