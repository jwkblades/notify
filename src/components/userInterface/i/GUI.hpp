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

#ifndef __NOTIFY_GUI_H
#define __NOTIFY_GUI_H

#include "Configuration.hpp"

#include <gtk/gtk.h>
#include <libnotify/notify.h>

/**
 * A single placeholder for all graphical interactions
 */
class GUI
{
public:
    /**
     * Ctor
     *
     * @param argc The number of arguments
     * @param argv The arguments
     * @param cfg The configuration
     */
    GUI(int argc, char** argv, const Configuration& cfg);

    /**
     * Dtor
     *
     * Meant to uninitialize the notification
     */
    ~GUI(void);

    /**
     * @return True if the GUI is initialized and ready to run, false otherwise
     */
    operator bool(void) const;

    /**
     * Display the GUI notification
     */
    void show(void);

    /**
     * Main loop for the GUI
     */
    void main(void);

    /**
     * Quit out of the main loop of the GUI
     */
    void quit(void);

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

private:
    NotifyNotification* mNotification;
    const Configuration& mConfig;
};

#endif
