#include "Configuration.hpp"
#include "Utilities.hpp"

#include <iostream>
#include <string>
#include <thread>

#include <gtk/gtk.h>
#include <libnotify/notify.h>

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
