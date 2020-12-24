#include "Application.hpp"

#include <gtk/gtk.h>
#include <signal.h>

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

    struct sigaction hupOld = {};
    struct sigaction hupNew = {};
    hupNew.sa_handler = [](int) -> void {
        defaultExit();
    };
    sigaction(SIGHUP, &hupNew, &hupOld);

    struct sigaction rt0Old = {};
    struct sigaction rt0New = {};
    rt0New.sa_handler = [](int) -> void {
        Application::instance()->closeSignal();
    };
    sigaction(SIGRTMIN, &rt0New, &rt0Old);

    struct sigaction rt1Old = {};
    struct sigaction rt1New = {};
    rt1New.sa_handler = [](int) -> void {
        Application::instance()->closeFirstOption();
    };
    sigaction(SIGRTMIN+1, &rt1New, &rt1Old);

    struct sigaction rt2Old = {};
    struct sigaction rt2New = {};
    rt2New.sa_handler = [](int) -> void {
        Application::instance()->closeSecondOption();
    };
    sigaction(SIGRTMIN+2, &rt2New, &rt2Old);

    struct sigaction rt3Old = {};
    struct sigaction rt3New = {};
    rt3New.sa_handler = [](int) -> void {
        Application::instance()->closeThirdOption();
    };
    sigaction(SIGRTMIN+3, &rt3New, &rt3Old);

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

void Application::closeFirstOption(void) const
{
    if (mConfig.optIndex < 1)
    {
        closeSignal();
        return;
    }
    close(mNotification, (char*)mConfig.values[0], NULL);
}

void Application::closeSecondOption(void) const
{
    if (mConfig.optIndex < 2)
    {
        closeFirstOption();
        return;
    }
    close(mNotification, (char*)mConfig.values[1], NULL);
}

void Application::closeThirdOption(void) const
{
    if (mConfig.optIndex < 3)
    {
        closeSecondOption();
        return;
    }
    close(mNotification, (char*)mConfig.values[2], NULL);
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