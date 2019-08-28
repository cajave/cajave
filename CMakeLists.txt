project("Tree Model")
set(EXE_NAME "treemodel")

cmake_minimum_required(VERSION 2.8.11)

# Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Added by Caojun 2019.4.20(
# 或者在命令行加入 
# -D Qt5Core_DIR:PATH=D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Core
# -D Qt5Widgets_DIR:PATH=D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Widgets
# -D Qt5Gui_DIR:PATH=D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Gui
# )
#set(CMAKE_PREFIX_PATH "D:\\Qt5.11.2\\msvc2017_64")

option(QT5_BUILD "Force Qt5 build" ON)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(HEADERS 
  mainwindow.h
  treemodel.h
  abstracttreeitem.h
  abstracttreemodel.h
)

set(SOURCES
  main.cpp
  mainwindow.cpp
  treemodel.cpp
  abstracttreeitem.cpp
  abstracttreemodel.cpp
)

set(FORMS
  mainwindow.ui
)

if(QT5_BUILD)
  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)

  include_directories(${Qt5Core_INCLUDE_DIRS})
  include_directories(${Qt5Gui_INCLUDE_DIRS})
  include_directories(${Qt5Widgets_INCLUDE_DIRS})

  add_definitions(${Qt5Core_DEFINITIONS})
  add_definitions(${Qt5Gui_DEFINITIONS})
  add_definitions(${Qt5Widgets_DEFINITIONS})
  add_definitions(-DHAVE_QT5)

  qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
  qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})

  qt4_wrap_cpp(MOC_SOURCES ${HEADERS})
  qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

add_executable(${EXE_NAME} WIN32 MACOSX_BUNDLE ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES})

if(QT5_BUILD)
  target_link_libraries(${EXE_NAME} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES})
else()
  target_link_libraries(${EXE_NAME} ${QT_LIBRARIES})
endif()
