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

#ifndef __NOTIFY_UTILITIES_H
#define __NOTIFY_UTILITIES_H

#include <gtk/gtk.h>
#include <libnotify/notify.h>

/**
 * The default notification exit handler.
 */
void defaultExit(void);

/**
 * Called when the notification was closed via user interactions, meant to
 * handle that user interaction and exit the application.
 *
 * @param Unused, a pointer to the notification that was interacted with.
 * @param action The action that was taken.
 * @param Unused, additional information.
 */
void close(NotifyNotification*, char* action, gpointer);

/**
 * Print usage text.
 */
void usage(void);

/**
 * A semi-busy loop (sleeps most of the time), waiting for application exit to
 * be requested, either through user interactions, or inaction causing the
 * notification to timeout.
 *
 * @param timeoutMinutes How many minutes the notification should remain open
 *     for before automatically closing and returning a default status.
 */
void timeoutThreadFunctor(size_t timeoutMinutes);

#endif
