@echo off

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