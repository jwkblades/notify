/*
 *    Copyright 2020, James Blades <jwkblades+git@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __NOTIFY_APPLICATION_H
#define __NOTIFY_APPLICATION_H

#include "Configuration.hpp"
#include "Utilities.hpp"

#include <libnotify/notify.h>

#include <thread>

/**
 * Main application class
 */
class Application
{
public:
    /**
     * Construct an application
     *
     * @param argc The count of arguments being passed in
     * @param argv The arguments themselves
     */
    Application(int argc, char** argv);

    /**
     * Destructor
     */
    ~Application(void);

    /**
     * Get the application global instance, set up by the first Application to
     * be constructed.
     *
     * @param self The instance
     * @return self (or, the previously set instance), only the first instance
     *     set succeeds.
     */
    static Application* instance(Application* self = NULL);

    /**
     * Callback used when we are told to close (by a SIGHUP, or SIGRTMIN)
     */
    void closeSignal(void) const;

    /**
     * Callback used when the first option signal is sent (SIGRTMIN+1)
     */
    void closeFirstOption(void) const;

    /**
     * Callback used when the second option signal is sent (SIGRTMIN+2)
     */
    void closeSecondOption(void) const;

    /**
     * Callback used when the third option signal is sent (SIGRTMIN+3)
     */
    void closeThirdOption(void) const;

    /**
     * The main application loop, doesn't return until the application is done.
     *
     * @return The return code of the application
     */
    int main(void);
private:
    bool mReady;
    NotifyNotification* mNotification;
    Configuration mConfig;
    std::thread mTimerThread;
    std::string mWallMessage;

    /**
     * Actually set up all the handlers
     */
    void setupSignalHandlers(void) const;
};

#endif
