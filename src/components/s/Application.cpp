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
        self->setupSignalHandlers();
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

    notify_notification_show(mNotification, NULL);
    mTimerThread = std::thread(timeoutThreadFunctor, mConfig.timeoutMinutes);

    gtk_main();
    mTimerThread.join();

    return 0;
}

void Application::setupSignalHandlers(void) const
{
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
}
