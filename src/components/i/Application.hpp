#ifndef __NOTIFY_APPLICATION_H
#define __NOTIFY_APPLICATION_H

#include "Configuration.hpp"
#include "Utilities.hpp"

#include <libnotify/notify.h>

#include <thread>

class Application
{
public:
    Application(int argc, char** argv);
    ~Application(void);

    static Application* instance(Application* self = NULL);

    void closeSignal(void) const;

    int main(void);
private:
    bool mReady;
    NotifyNotification* mNotification;
    Configuration mConfig;
    std::thread mTimerThread;
};

#endif
