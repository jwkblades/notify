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
    std::cerr << "Usage:" << std::endl
         << "    " << "notify [options] <TITLE> <DESCRIPTION>" << std::endl
         << std::endl
         << "Options:" << std::endl
         << "    " << "--option|-o     An option, up to 3 are allowed. Options are the display string shown on a notification button." << std::endl
         << "    " << "--value|-v      An value, up to 3 are allowed. Values are the returned value when a notification option is selected." << std::endl
         << "    " << "--timeout|-t    Timeout for the notification, in minutes." << std::endl
         << "    " << "--icon|-i       The desired icon name to be used." << std::endl;
}

class Configuration
{
public:
    const char* title;
    const char* description;
    const char* icon;
    const char* options[3];
    const char* values[3];
    size_t timeoutMinutes;
    bool mIsValid;
    int optIndex;

    Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions):
        title(NULL),
        description(NULL),
        icon("dialog-information"),
        options(),
        values(),
        timeoutMinutes(0),
        mIsValid(false)
    {
        int valIndex = 0;
        int index(1);

        title = argv[index];
        description = argv[index + 1];

        while (1)
        {
            int c = getopt_long(argc, argv, shortOptions, longOptions, NULL);
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
                    timeoutMinutes = std::stoull(optarg);
                    index += 2;
                    break;
                case 'i':
                    icon = optarg;
                    index += 2;
                    break;
                default:
                    return;
            }
        }

        if (index + 2 == argc)
        {
            mIsValid = true;
        }

        if (optIndex > valIndex)
        {
            std::cerr << "The number of options and values should be the same. Using the lesser." << std::endl;
            optIndex = valIndex;
        }
    }

    [[nodiscard]] inline bool valid(void) const
    {
        return mIsValid;
    }
};

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

    for (int i = 0; i < cfg.optIndex; ++i)
    {
        notify_notification_add_action(notification, cfg.values[i], cfg.options[i], close, NULL, NULL);
    }

    notify_notification_add_action(notification, "default", "OK", close, NULL, NULL);
    g_signal_connect(G_OBJECT(notification), "closed", defaultExit, notification);
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);

    std::thread timerThread(timeoutThreadFunctor, cfg.timeoutMinutes);
	notify_notification_show(notification, NULL);

    gtk_main();
    timerThread.join();
	notify_uninit();

	return 0;
}
