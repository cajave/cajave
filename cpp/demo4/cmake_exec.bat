@echo off

:: cmake -i 为交互式(3.8已经不支持-i)
:: config.h中
:: /* #undef USE_MYMATH */ 	-D USE_MYMATH:bool=OFF
:: #define USE_MYMATH 		-D USE_MYMATH:bool=ON
cmake -G "Visual Studio 15 2017 Win64" -S . -B build

if not exist build (
	exit 1
)

cd build
echo switch into %cd%

set MSBUILDER="D:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.com"
%MSBUILDER% Demo.sln /build "Debug"

cmd -K

pause