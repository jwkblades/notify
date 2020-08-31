#include <ios>
#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>

#include <getopt.h>
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

void user_function(void)
{
    std::cout << "OK" << std::endl;
    teardown();
}

void Close(NotifyNotification*, char* action, gpointer)
{
    if (std::string("default") == action)
    {
        user_function();
        return;
    }

    std::cout << action << std::endl;
    teardown();
}

void usage(void)
{
    std::cerr << "Usage:" << std::endl
         << "    " << "notify [options] <TITLE> <DESCRIPTION>" << std::endl
         << std::endl
         << "Options:" << std::endl
         << "    " << "--now-text|-n         The text to display for the default 'now' option, indicating to take immediate action." << std::endl
         << "    " << "--later-text|-l       The text to display for the later action, indicating that a deferral is desired." << std::endl
         << "    " << "--deferral-period|-d  The deferral period, in hours, until we should try again." << std::endl
         << "    " << "--timeout|-t          Timeout for the notification, in minutes." << std::endl
         << "    " << "--icon|-i             The desired icon name to be used." << std::endl;
}

int main(int argc, char** argv)
{
    std::string title("");
    std::string description("");
    std::string nowOption("Now");
    std::string laterOption("Later");
    std::string laterValue("1");
    std::string nowValue("NOW");
    std::string icon("dialog-information");
    size_t timeoutMinutes(15);
    bool laterSet(false);

    static struct option longOptions[] = {
        {"now-text", 1, 0, 'n'},
        {"later-text", 1, 0, 'l'},
        {"deferral-period", 1, 0, 'd'},
        {"timeout", 1, 0, 't'},
        {"icon", 1, 0, 'i'}
    };

    int optionIndex(0);
    int index(1);

    while (1)
    {
        int c = getopt_long(argc, argv, "n:l:d:t:", longOptions, &optionIndex);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'n':
                nowOption = optarg;
                index += 2;
                break;
            case 'l':
                laterOption = optarg;
                index += 2;
                break;
            case 'd':
                if (!laterSet)
                {
                    laterValue = optarg;
                    laterSet = true;
                }
                index += 2;
                break;
            case 't':
                timeoutMinutes = std::atoll(optarg);
                index += 2;
                break;
            case 'i':
                icon = optarg;
                index += 2;
                break;
            default:
                usage();
                return 1;
        }
    }

    if (index + 2 != argc)
    {
        usage();
        return 1;
    }

    title = argv[index];
    description = argv[index + 1];


    gtk_init(&argc, &argv);

	notify_init("agentNotifier");
	NotifyNotification* notification = notify_notification_new(title.c_str(), description.c_str(), icon.c_str());

    notify_notification_add_action(notification, laterValue.c_str(), laterOption.c_str(), Close, NULL, NULL);
    notify_notification_add_action(notification, nowValue.c_str(), nowOption.c_str(), Close, NULL, NULL);

    notify_notification_add_action(notification, "default", "OK", Close, NULL, NULL);
    g_signal_connect(G_OBJECT(notification), "closed", user_function, notification);

    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);

    std::thread timerThread([timeoutMinutes]() -> void {
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
            user_function();
        }
    });
	notify_notification_show(notification, NULL);

    gtk_main();
    timerThread.join();
	notify_uninit();

	return 0;
}
