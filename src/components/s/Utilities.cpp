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

#include "Utilities.hpp"
#include "UtilitiesPrivate.hpp"

#include <string>
#include <chrono>
#include <iostream>
#include <unistd.h>

void defaultExit(void)
{
    std::cout << "DEFAULT" << std::endl;
    teardown();
}

void close(NotifyNotification*, char* action, gpointer)
{
    if (std::string("default") == action)
    {
        defaultExit();
        return;
    }

    std::cout << action << std::endl;
    teardown();
}

void usage(void)
{
    std::cerr << R"__(Usage:
    notify [options] <TITLE> <DESCRIPTION>

Options:
    --option|-o     An option, up to 3 are allowed. Options are displayed in the notification buttons.
    --value|-v      A value, up to 3 are allowed. Values are printed when a notification option is selected.
    --timeout|-t    Notification lifetime, in minutes.
    --icon|-i       The desired icon name to be displayed in the notification.
    )__" << std::endl;
}


void timeoutThreadFunctor(size_t timeoutMinutes)
{
    std::chrono::high_resolution_clock::time_point timeoutTime = std::chrono::high_resolution_clock::now();
    timeoutTime += std::chrono::minutes(timeoutMinutes);
    bool timedOut = false;

    while (!notificationExitNow())
    {
        if (std::chrono::high_resolution_clock::now() >= timeoutTime)
        {
            timedOut = true;
            break;
        }

        usleep(100 * 1000); // 100ms
    }

    if (timedOut)
    {
        defaultExit();
    }
}

// PRIVATE
bool notificationExitNow(bool set)
{
    static bool applicationValue = false;
    if (set)
    {
        applicationValue = set;
    }
    return applicationValue;
}

void teardown(void)
{
    notificationExitNow(true);
    gtk_main_quit();
}

