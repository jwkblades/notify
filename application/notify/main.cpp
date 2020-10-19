#include <iostream>
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

void defaultExit(void)
{
    std::cout << "OK" << std::endl;
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
    std::cerr << "Usage:" << std::endl
         << "    " << "notify [options] <TITLE> <DESCRIPTION>" << std::endl
         << std::endl
         << "Options:" << std::endl
         << "    " << "--option|-o     An option, up to 3 are allowed. Options are the display string shown on a notification button." << std::endl
         << "    " << "--value|-v      An value, up to 3 are allowed. Values are the returned value when a notification option is selected." << std::endl
         << "    " << "--timeout|-t    Timeout for the notification, in minutes." << std::endl
         << "    " << "--icon|-i       The desired icon name to be used." << std::endl;
}

int main(int argc, char** argv)
{
    const char* title(NULL);
    const char* description(NULL);
    const char* options[3] = {NULL, NULL, NULL};
    const char* values[3] = {NULL, NULL, NULL};
    int optIndex = 0;
    int valIndex = 0;
    std::string icon("dialog-information");
    size_t timeoutMinutes(15);

    static struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    int index(1);

    while (1)
    {
        int c = getopt_long(argc, argv, "i:o:t:v:", longOptions, NULL);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'o':
                if (optIndex >= 3)
                {
                    std::cerr << "Too many options specified. Only 3 allowed." << std::endl;
                    break;
                }
                options[optIndex++] = optarg;
                index += 2;
                break;
            case 'v':
                if (valIndex >= 3)
                {
                    std::cerr << "Too many values specified. Only 3 allowed." << std::endl;
                    break;
                }
                values[valIndex++] = optarg;
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

    if (optIndex > valIndex)
    {
        std::cerr << "The number of options and values should be the same. Using the lesser." << std::endl;
        optIndex = valIndex;
    }

    gtk_init(&argc, &argv);

	notify_init("agentNotifier");
	NotifyNotification* notification = notify_notification_new(title, description, icon.c_str());

    for (int i = 0; i < optIndex; ++i)
    {
        notify_notification_add_action(notification, values[i], options[i], close, NULL, NULL);
    }

    notify_notification_add_action(notification, "default", "OK", close, NULL, NULL);
    g_signal_connect(G_OBJECT(notification), "closed", defaultExit, notification);

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
            defaultExit();
        }
    });
	notify_notification_show(notification, NULL);

    gtk_main();
    timerThread.join();
	notify_uninit();

	return 0;
}
