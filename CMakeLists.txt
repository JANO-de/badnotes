cmake_minimum_required(VERSION 3.21)
project(inkwell VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.6 REQUIRED COMPONENTS Quick QuickControls2 Qml Gui Network)
qt_standard_project_setup(REQUIRES 6.6)

set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_executable(inkwell src/main.cpp src/InkCanvas.cpp src/InkCanvas.h
  src/InkPage.cpp src/InkPage.h src/Notebook.cpp src/Notebook.h src/Tools.h src/Updater.cpp src/Updater.h
  src/Library.cpp src/Library.h src/PageRenderer.cpp src/PageRenderer.h
  src/PageThumb.cpp src/PageThumb.h src/CoverProvider.h)
qt_add_qml_module(inkwell
  URI Inkwell VERSION 1.0
  QML_FILES qml/Main.qml qml/Theme.qml qml/GlassPanel.qml qml/GlassButton.qml qml/FloatingToolbar.qml
            qml/PageBar.qml qml/PageSidebar.qml qml/NotebookView.qml
            qml/LibraryView.qml qml/AskPopup.qml
)
set(INKWELL_BUILD "dev" CACHE STRING "build id (commit sha) used by the self-updater")
target_compile_definitions(inkwell PRIVATE INKWELL_BUILD="${INKWELL_BUILD}")
target_include_directories(inkwell PRIVATE src)
target_link_libraries(inkwell PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Qml Qt6::Gui Qt6::Network)
install(TARGETS inkwell RUNTIME DESTINATION bin)
install(FILES packaging/inkwell.desktop DESTINATION share/applications)
install(FILES packaging/inkwell.svg DESTINATION share/icons/hicolor/scalable/apps RENAME inkwell.svg)
