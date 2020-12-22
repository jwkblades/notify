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

#ifndef __NOTIFY_PRIVATE_UTILITIES_H
#define __NOTIFY_PRIVATE_UTILITIES_H

/**
 * Exit the notification.
 *
 * @param set Whether to set the exit value. Once set, the exit value is
 *     maintained.
 * @return bool Whether we should exit the application at this point.
 */
bool notificationExitNow(bool set = false);

/**
 * A function to pair the teardown requirements for a notification and then the
 * application as a whole.
 */
void teardown(void);

#endif
