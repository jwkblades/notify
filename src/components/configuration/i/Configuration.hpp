#ifndef __NOTIFY_CONFIGURATION_H
#define __NOTIFY_CONFIGURATION_H

#include <cstdlib>
#include <getopt.h>

struct Configuration
{
public:
    static const int MAX_OPTIONS = 3;

    const char* title;
    const char* description;
    const char* icon;
    const char* options[MAX_OPTIONS];
    const char* values[MAX_OPTIONS];
    size_t timeoutMinutes;
    bool mIsValid;
    int optIndex;

    Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions);

    [[nodiscard]] inline bool valid(void) const
    {
        return mIsValid;
    }
};

#endif
