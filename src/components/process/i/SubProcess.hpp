#ifndef __NOTIFY_SUB_PROCESS_H
#define __NOTIFY_SUB_PROCESS_H

#include <sys/types.h>

#include <exception>
#include <string>

#define SUB_PROCESS_TEMPLATE_LENGTH 6

class SubProcessTemplateException: public std::exception
{
public:
    const char* what(void) const noexcept override;
};

class SubProcess
{
public:
    SubProcess(const char* filenameTemplate, const char* script);
    ~SubProcess(void);

    [[nodiscard]] inline pid_t pid(void) const
    {
        return mPid;
    }

    [[nodiscard]] inline int returnCode(void) const
    {
        return mRc;
    }

    const std::string& stdout(void);
    const std::string& stderr(void);

    int run(void);
    int wait(void);
private:
    pid_t mPid;
    char* mFilename;
    const char* const mScript;
    int mStdout[2];
    int mStderr[2];
    int mRc;
    std::string mStdoutString;
    std::string mStderrString;
};

#endif
