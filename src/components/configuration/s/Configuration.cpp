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

#include "Configuration.hpp"

#include <iostream>

Configuration::Configuration(void):
    title(NULL),
    description(NULL),
    icon("dialog-information"),
    options(),
    values(),
    timeoutMinutes(0),
    optIndex(0),
    mIsValid(false)
{
    // empty
}

void Configuration::parse(int argc, char** argv, const char* shortOptions, struct option* longOptions)
{
    int valIndex = 0;
    int index = 1;

    // Reset getopt globals to make this safe for multi-use (such as in unit
    // test environments).
    optind = 0;
    opterr = 0;
    optopt = 0;

    while (true)
    {
        int c = getopt_long(argc, argv, shortOptions, longOptions, NULL);
        if (c == -1)
        {
            if (index + 2 != argc)
            {
                std::cerr << "No option recognized. Breaking at index " << index << ", of " << argc << std::endl;
            }
            break;
        }

        switch (c)
        {
        case 'o':
            if (optIndex >= MAX_OPTIONS)
            {
                std::cerr << "Too many options specified. Only " << MAX_OPTIONS << " allowed." << std::endl;
            }
            else
            {
                options[optIndex++] = optarg;
            }
            index += 2;
            break;
        case 'v':
            if (valIndex >= MAX_OPTIONS)
            {
                std::cerr << "Too many values specified. Only " << MAX_OPTIONS << " allowed." << std::endl;
            }
            else
            {
                values[valIndex++] = optarg;
            }
            index += 2;
            break;
        case 't':
            timeoutMinutes = std::stoull(optarg);
            index += 2;
            break;
        case 'i':
            icon = optarg;
            index += 2;
            break;
        default:
            return;
        }
    }

    if (index + 2 == argc)
    {
        mIsValid = true;

        title = argv[index];
        description = argv[index + 1];
    }

    if (optIndex != valIndex)
    {
        std::cerr << "The number of options and values should be the same. Using the lesser." << std::endl;
        optIndex = std::min(optIndex, valIndex);
    }
}
