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
