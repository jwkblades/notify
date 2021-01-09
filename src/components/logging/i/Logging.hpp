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

#ifndef __NOTIFY_LOGGING_H
#define __NOTIFY_LOGGING_H

#include <sstream>

#include <syslog.h>

/**
 * Primary log interface
 */
class Log
{
public:
    /**
     * Start a new log message, of the given priority
     *
     * @param priority The priority of the log message (see `man 3 syslog` for
     *     more information).
     */
    Log(int priority);

    /**
     * Dtor, responsible for actually writing out the log message to the system.
     */
    ~Log(void);

    [[nodiscard]] inline std::string str(void) const
    {
        return mStream.str();
    }

    /**
     * A nice forwarder, which forwards anything it is fed to an internal log
     * stream.
     *
     * @param val The item to log
     * @return Ourselves, for call chaining.
     */
    template<typename T>
    Log& operator<<(const T& val);

private:
    int mPriority;
    std::stringstream mStream;
};

template<typename T>
Log& Log::operator<<(const T& val)
{
    mStream << val;
    return *this;
}
#endif
