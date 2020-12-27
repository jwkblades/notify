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

#ifndef __NOTIFY_LOGGOING_UTILITY_PRIVATE_H
#define __NOTIFY_LOGGOING_UTILITY_PRIVATE_H

#include <syslog.h>

/**
 * Set up the initial logging connection, and be sure that it is properly
 * closed down at the end of program execution.
 */
void ensureLoggingInitialized(void);

#endif
