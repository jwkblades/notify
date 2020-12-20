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

Configuration::Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions):
    title(NULL),
    description(NULL),
    icon("dialog-information"),
    options(),
    values(),
    timeoutMinutes(0),
    optIndex(0),
    mIsValid(false)
{
    int valIndex = 0;
    int index(1);

    while (1)
    {
        int c = getopt_long(argc, argv, shortOptions, longOptions, NULL);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'o':
                if (optIndex >= MAX_OPTIONS)
                {
                    std::cerr << "Too many options specified. Only 3 allowed." << std::endl;
                    break;
                }
                options[optIndex++] = optarg;
                index += 2;
                break;
            case 'v':
                if (valIndex >= MAX_OPTIONS)
                {
                    std::cerr << "Too many values specified. Only 3 allowed." << std::endl;
                    break;
                }
                values[valIndex++] = optarg;
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

    if (optIndex > valIndex)
    {
        std::cerr << "The number of options and values should be the same. Using the lesser." << std::endl;
        optIndex = valIndex;
    }
}
