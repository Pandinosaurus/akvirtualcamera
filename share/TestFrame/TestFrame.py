#!/usr/bin/env python
#
# akvirtualcamera, virtual camera for Mac and Windows.
# Copyright (C) 2020  Gonzalo Exequiel Pedone
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

import sys
from PyQt5 import QtWidgets, QtQml, QtQuick


if __name__ =='__main__':
    app = QtWidgets.QApplication(sys.argv);
    engine = QtQml.QQmlApplicationEngine()
    engine.load("TestFrame.qml")

    def capture():
        for obj in engine.rootObjects():
            image = obj.grabWindow()
            image.save("TestFrame.bmp")
            obj.close()

    for obj in engine.rootObjects():
        obj.sceneGraphInitialized.connect(capture)

    app.exec_()
