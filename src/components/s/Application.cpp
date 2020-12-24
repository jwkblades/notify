#include "Application.hpp"

#include <gtk/gtk.h>

Application::Application(int argc, char** argv):
    mReady(false),
    mNotification(NULL),
    mConfig()
{
    static struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    mConfig.parse(argc, argv, "i:o:t:v:", longOptions);
    if (!mConfig.valid())
    {
        usage();
        return;
    }

    gtk_init(&argc, &argv);
    notify_init(mConfig.title);
    mNotification = notify_notification_new(mConfig.title, mConfig.description, mConfig.icon);

    for (int i = 0; i < mConfig.optIndex && i < mConfig.MAX_OPTIONS; ++i)
    {
        notify_notification_add_action(mNotification, mConfig.values[i], mConfig.options[i], close, NULL, NULL);
    }

    notify_notification_add_action(mNotification, "default", "OK", close, NULL, NULL);
    g_signal_connect(G_OBJECT(mNotification), "closed", defaultExit, mNotification);
    notify_notification_set_urgency(mNotification, NOTIFY_URGENCY_CRITICAL);

    notify_notification_show(mNotification, NULL);
    mTimerThread = std::thread(timeoutThreadFunctor, mConfig.timeoutMinutes);

    instance(this);
    mReady = true;
}

Application::~Application(void)
{
    notify_notification_close(mNotification, NULL);
    notify_uninit();
}

Application* Application::instance(Application* self)
{
    static Application* oInstance = NULL;
    if (self && !oInstance)
    {
        oInstance = self;
    }
    return oInstance;
}

void Application::closeSignal(void) const
{
    static const char* action = "default";
    close(mNotification, (char*)action, NULL);
}

int Application::main(void)
{
    if (!mReady)
    {
        return 1;
    }

    gtk_main();
    mTimerThread.join();

    return 0;
}
