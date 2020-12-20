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
#include "Utilities.hpp"

#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <thread>

int main(int argc, char** argv)
{
    static struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration cfg(argc, argv, "i:o:t:v:", longOptions);
    if (!cfg.valid())
    {
        usage();
        return 1;
    }

    gtk_init(&argc, &argv);
    notify_init(cfg.title);
    NotifyNotification* notification = notify_notification_new(cfg.title, cfg.description, cfg.icon);

    for (int i = 0; i < cfg.optIndex && i < cfg.MAX_OPTIONS; ++i)
    {
        notify_notification_add_action(notification, cfg.values[i], cfg.options[i], close, NULL, NULL);
    }

    notify_notification_add_action(notification, "default", "OK", close, NULL, NULL);
    g_signal_connect(G_OBJECT(notification), "closed", defaultExit, notification);
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);

    notify_notification_show(notification, NULL);
    std::thread timerThread(timeoutThreadFunctor, cfg.timeoutMinutes);

    gtk_main();
    timerThread.join();
    notify_notification_close(notification, NULL);
    notify_uninit();

    return 0;
}
