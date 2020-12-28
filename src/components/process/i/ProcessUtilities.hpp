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

#ifndef __NOTIFY_PROCESS_UTILITIES_H
#define __NOTIFY_PROCESS_UTILITIES_H

#include <string>

/**
 * Sanitize a string for bash scripts, ensuring that it will not be capable of
 * breaking out and injecting arbitrary commands into a bash sub process.
 *
 * @param The string to sanitize
 * @return The sanitized string
 */
std::string sanitizeForBash(const char* s);

#endif

