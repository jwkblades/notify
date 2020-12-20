#include "Configuration.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>

bool notificationExitNow(bool set = false)
{
    static bool applicationValue = false;
    if (set)
    {
        applicationValue = set;
    }
    return applicationValue;
}

void teardown(void)
{
    notificationExitNow(true);
    gtk_main_quit();
}

void defaultExit(void)
{
    std::cout << "DEFAULT" << std::endl;
    teardown();
}

void close(NotifyNotification*, char* action, gpointer)
{
    if (std::string("default") == action)
    {
        defaultExit();
        return;
    }

    std::cout << action << std::endl;
    teardown();
}

void usage(void)
{
    std::cerr << R"__(Usage:
    notify [options] <TITLE> <DESCRIPTION>

Options:
    --option|-o     An option, up to 3 are allowed. Options are displayed in the notification buttons.
    --value|-v      A value, up to 3 are allowed. Values are printed when a notification option is selected.
    --timeout|-t    Notification lifetime, in minutes.
    --icon|-i       The desired icon name to be displayed in the notification.
    )__" << std::endl;
}


void timeoutThreadFunctor(size_t timeoutMinutes)
{
    std::chrono::high_resolution_clock::time_point timeoutTime = std::chrono::high_resolution_clock::now();
    timeoutTime += std::chrono::minutes(timeoutMinutes);
    bool timedOut = false;

    while (!notificationExitNow())
    {
        if (std::chrono::high_resolution_clock::now() >= timeoutTime)
        {
            timedOut = true;
            break;
        }

        usleep(100 * 1000); // 100ms
    }

    if (timedOut)
    {
        defaultExit();
    }
}

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
