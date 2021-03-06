REM akvirtualcamera, virtual camera for Mac and Windows.
REM Copyright (C) 2021  Gonzalo Exequiel Pedone
REM
REM akvirtualcamera is free software: you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation, either version 3 of the License, or
REM (at your option) any later version.
REM
REM akvirtualcamera is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM
REM You should have received a copy of the GNU General Public License
REM along with akvirtualcamera. If not, see <http://www.gnu.org/licenses/>.
REM
REM Web-Site: http://webcamoid.github.io/

set SOURCES_DIR=%cd%
set INSTALL_PREFIX=%SOURCES_DIR%\webcamoid-data

echo.
echo Building x64 virtual camera driver
echo.

mkdir build-x64
cd build-x64

cmake ^
    -G "%CMAKE_GENERATOR%" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
    ..
cmake --build . --config Release
cmake --build . --config Release --target install

cd ..

echo.
echo Building x86 virtual camera driver
echo.

mkdir build-x86
cd build-x86

cmake ^
    -G "%CMAKE_GENERATOR%" ^
    -A Win32 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
    ..
cmake --build . --config Release
cmake --build . --config Release --target install

cd ..
