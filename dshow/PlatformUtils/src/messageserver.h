/* akvirtualcamera, virtual camera for Mac and Windows.
 * Copyright (C) 2020  Gonzalo Exequiel Pedone
 *
 * akvirtualcamera is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * akvirtualcamera is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with akvirtualcamera. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <limits>
#include <map>
#include <minwindef.h>

#include "messagecommons.h"
#include "VCamUtils/src/utils.h"

#define MSERVER_TIMEOUT_DEFAULT 0
#define MSERVER_TIMEOUT_MIN 1
#define MSERVER_TIMEOUT_MAX (std::numeric_limits<uint32_t>::max)()

namespace AkVCam
{
    class MessageServerPrivate;

    class MessageServer
    {
        public:
            enum ServerMode
            {
                ServerModeReceive,
                ServerModeSend
            };

            enum State
            {
                StateAboutToStart,
                StateStarted,
                StateAboutToStop,
                StateStopped
            };

            enum PipeState
            {
                PipeStateAvailable,
                PipeStateGone
            };

            AKVCAM_SIGNAL(StateChanged, State state)
            AKVCAM_SIGNAL(PipeStateChanged, PipeState state)

        public:
            MessageServer();
            MessageServer(const MessageServer &other) = delete;
            ~MessageServer();

            std::string pipeName() const;
            std::string &pipeName();
            void setPipeName(const std::string &pipeName);
            ServerMode mode() const;
            ServerMode &mode();
            void setMode(ServerMode mode);
            int checkInterval() const;
            int &checkInterval();
            void setCheckInterval(int checkInterval);
            void setHandlers(const std::map<uint32_t,
                             MessageHandler> &handlers);
            bool start(bool wait=false);
            void stop(bool wait=false);
            BOOL sendMessage(Message *message,
                             uint32_t timeout=MSERVER_TIMEOUT_MAX);
            BOOL sendMessage(const Message &messageIn,
                             Message *messageOut,
                             uint32_t timeout=MSERVER_TIMEOUT_MAX);
            static BOOL sendMessage(const std::string &pipeName,
                                    Message *message,
                                    uint32_t timeout=MSERVER_TIMEOUT_MAX);
            static BOOL sendMessage(const std::string &pipeName,
                                    const Message &messageIn,
                                    Message *messageOut,
                                    uint32_t timeout=MSERVER_TIMEOUT_MAX);

        private:
            MessageServerPrivate *d;
            friend class MessageServerPrivate;
    };
}

#endif // MESSAGESERVER_H
