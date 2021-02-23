#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Webcamoid, webcam capture application.
# Copyright (C) 2017  Gonzalo Exequiel Pedone
#
# Webcamoid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Webcamoid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
#
# Web-Site: http://webcamoid.github.io/

import os
from WebcamoidDeployTools import DTUtils


if __name__ =='__main__':
    system = DTUtils.Utils().system
    scriptDir = os.path.dirname(os.path.realpath(__file__))

    while True:
        scriptPath = os.path.join(scriptDir, 'deploy_{}.py'.format(system))
        
        if not os.path.exists(scriptPath):
            print('No valid deploy script found')
            exit()
            
        deploy = __import__('deploy_' + system).Deploy()

        if system == deploy.targetSystem:
            deploy.run()

            exit()

        system = deploy.targetSystem
