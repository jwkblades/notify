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

#ifndef __NOTIFY_CONFIGURATION_H
#define __NOTIFY_CONFIGURATION_H

#include <cstdlib>
#include <getopt.h>

#define CONFIGURATION_MAX_OPTIONS 3

/**
 * A Configuration, as represented by an application's command line arguments.
 *
 * Configuration is meant to take in all command line arguments passed into the
 * executable at runtime, as a result, it is expected that `argv[0]` is unused
 * by the configuration as that will be the application target (the executable
 * being run), and not a valid option.
 *
 * Additionally, Configuration uses `getopt` and `getopt_long` to do option
 * parsing, meaning that it is NOT thread-safe.
 */
struct Configuration
{
public:
    static const int MAX_OPTIONS = CONFIGURATION_MAX_OPTIONS;

    const char* title;
    const char* description;
    const char* icon;
    const char* options[MAX_OPTIONS];
    const char* values[MAX_OPTIONS];
    size_t timeoutMinutes;
    int optIndex;

    /**
     * Construct the application configuration
     *
     * @param argc The number of arguments being passed in.
     * @param argv The arguments
     * @param shortOptions The option string to be used for short options.
     * @param longOptions Long options to be used.
     */
    Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions);

    /**
     * Is the configuration valid?
     *
     * @return bool True if it is valid, false otherwise.
     */
    [[nodiscard]] inline bool valid(void) const
    {
        return mIsValid;
    }

private:
    bool mIsValid;
};

#endif
