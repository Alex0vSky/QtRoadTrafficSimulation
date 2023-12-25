# QtRoadTrafficSimulation
[![cpp-logo](https://img.shields.io/badge/C%2B%2B-v17-blue?logo=cplusplus)](
https://en.wikipedia.org/wiki/C++
)

Road traffic simulation using Qt 

@todo
- [x] roads in QGraphicsView, zoomable and movable
- [x] roads in QQuickItem, zoomable and movable
- [x] roads in QQuickPaintedItem, zoomable and movable
- [x] car animation in QGraphicsView
- [ ] car animation in QQuickItem
- [ ] car animation in QQuickPaintedItem

## Features
Traffic modeling with three found ways to display graphics in Qt:
 - `QGraphicsView + scene` and rendering in main thread;
 - `QQuickItem + updatePaintNode` and rendering in separate thread;
 - `QQuickPaintedItem + QPainter` and rendering in separate thread;

## Requirements
Qt 5.15

## Install
The application consists of one file. Does not require installation. 

## Usage
Zoomable and movable canvas

## Tests

## Build
Building the project and tests from the source code requires the presence of the __Microsoft Visual Studio 2019 Community__, and using *.sln and *.vcxproj project files.

## Contributing
Can ask questions. PRs are accepted. No requirements for contributing.

## Thanks
[Qt](https://www.qt.io/)
[BilHim/trafficSimulator](https://github.com/BilHim/trafficSimulator)
[yossidoctor/AI-Traffic-Lights-Controller](https://github.com/yossidoctor/AI-Traffic-Lights-Controller)
[woboq/verdigris](https://github.com/woboq/verdigris)

## License
See the [LICENSE](https://github.com/Alex0vSky/QtRoadTrafficSimulation/blob/main/LICENSE) file for license rights and limitations (MIT).
