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
#include "Logging.hpp"
#include "ProcessUtilities.hpp"
#include "SubProcess.hpp"

#include <algorithm>

#include <signal.h>
#include <unistd.h>

Application::Application(int argc, char** argv):
    mReady(false),
    mConfig(),
    mGui(NULL)
{
    static const std::string find("\"");
    static const std::string repl("\\\"");
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

    Log startupLog(LOG_NOTICE);

    startupLog << "Notify is running in process " << getpid() << ", send it the following signals to interact:\r\n" << "    SIGRTMIN:   Default action\r\n";
    for (int i = 0; i < mConfig.optIndex && i < mConfig.MAX_OPTIONS; ++i)
    {
        startupLog << "    SIGRTMIN+" << (i + 1) << ": " << sanitizeForBash(mConfig.options[i]) << "\r\n";
    }

    mWallMessage = startupLog.str();
    mGui = new GUI(argc, argv, mConfig);

    instance(this);
    mReady = true;
}

Application::~Application(void)
{
    delete mGui;
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

int Application::main(void)
{
    if (!mReady)
    {
        return 1;
    }

    std::string wall = "#!/bin/bash\n\nwall \"\"\"" + mWallMessage + "\"\"\"";
    SubProcess proc("/tmp/notify-wall-message-XXXXXX", wall.c_str());
    proc.run();

    mGui->show();
    mTimerThread = std::thread(timeoutThreadFunctor, mConfig.timeoutMinutes);

    proc.wait();
    mGui->main();
    mTimerThread.join();

    return 0;
}

void Application::quit(void)
{
    mGui->quit();
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
        Application::instance()->mGui->closeSignal();
    };
    sigaction(SIGRTMIN, &rt0New, &rt0Old);

    struct sigaction rt1Old = {};
    struct sigaction rt1New = {};
    rt1New.sa_handler = [](int) -> void {
        Application::instance()->mGui->closeFirstOption();
    };
    sigaction(SIGRTMIN+1, &rt1New, &rt1Old);

    struct sigaction rt2Old = {};
    struct sigaction rt2New = {};
    rt2New.sa_handler = [](int) -> void {
        Application::instance()->mGui->closeSecondOption();
    };
    sigaction(SIGRTMIN+2, &rt2New, &rt2Old);

    struct sigaction rt3Old = {};
    struct sigaction rt3New = {};
    rt3New.sa_handler = [](int) -> void {
        Application::instance()->mGui->closeThirdOption();
    };
    sigaction(SIGRTMIN+3, &rt3New, &rt3Old);
}
