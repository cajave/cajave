@echo off
set src_dir=.
set build_dir=./build
set optionG="Visual Studio 15 2017 Win64"
set optionD1=Qt5Core_DIR:PATH="D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Core"
set optionD2=Qt5Widgets_DIR:PATH="D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Widgets"
set optionD3=Qt5Gui_DIR:PATH="D:/Qt5.11.2/msvc2017_64/lib/cmake/Qt5Gui"
::如果不用-D选项，则需要在CMakeList中添加set(CMAKE_PREFIX_PATH "D:\\Qt5.11.2\\msvc2017_64")指定Qt相关路径
cmake -G %optionG% -D %optionD1% -D %optionD2% -D %optionD3% -S "%src_dir%" -B "%build_dir%"
"D:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.com" "build/Tree Model.sln" /build Debug
pause