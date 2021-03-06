#!/bin/bash

# akvirtualcamera, virtual camera for Mac and Windows.
# Copyright (C) 2021  Gonzalo Exequiel Pedone
#
# akvirtualcamera is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# akvirtualcamera is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with akvirtualcamera. If not, see <http://www.gnu.org/licenses/>.
#
# Web-Site: http://webcamoid.github.io/

SOURCES_DIR=${PWD}
INSTALL_PREFIX=${SOURCES_DIR}/webcamoid-data
INSTALL_PREFIX_W=$(cygpath -w "${INSTALL_PREFIX}")
PACKAGES_DIR=${SOURCES_DIR}/webcamoid-packages
PACKAGES_DIR_W=$(cygpath -w "${PACKAGES_DIR}")

git clone https://github.com/webcamoid/DeployTools.git

/c/msys64/mingw32/bin/strip "${INSTALL_PREFIX}"/x86/*
/c/msys64/mingw64/bin/strip "${INSTALL_PREFIX}"/x64/*

export PATH="/c/Program Files (x86)/NSIS:${PATH}"

PYTHONPATH=${SOURCES_DIR}/DeployTools
BUILD_PATH=${SOURCES_DIR}/build-x64
BUILD_PATH_W=$(cygpath -w "${BUILD_PATH}")
python DeployTools/deploy.py \
    -d "${INSTALL_PREFIX_W}" \
    -c "${BUILD_PATH_W}/package_info.conf" \
    -o "${PACKAGES_DIR_W}"
