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
    int optIndex;

    /**
     * Construct the application configuration
     *
     * @param argc The number of arguments being passed in.
     * @param argv The arguments
     * @param shortOptions The option string to be used for short options.
     * @param longOptions Long options to be used.
     */
    Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions);

    /**
     * Is the configuration valid?
     *
     * @return bool True if it is valid, false otherwise.
     */
    [[nodiscard]] inline bool valid(void) const
    {
        return mIsValid;
    }

private:
    bool mIsValid;
};

#endif
