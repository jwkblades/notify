#ifndef __NOTIFY_SUB_PROCESS_H
#define __NOTIFY_SUB_PROCESS_H

#include <sys/types.h>

#include <exception>
#include <string>

#define SUB_PROCESS_TEMPLATE_LENGTH 6

/**
 * An exception that will be thrown by SubProcess's constructor if the
 * filenameTemplate is incorrectly formatted.
 */
class SubProcessTemplateException: public std::exception
{
public:
    /**
     * @return The exception reason.
     */
    const char* what(void) const noexcept override;
};

/**
 * SubProcess class, meant to take a script and be used to execute it.
 */
class SubProcess
{
public:
    /**
     * Constructor
     *
     * @param filenameTemplate A string template in the form of
     *     "/path/to/file/named-XXXXXX". It is worth noting that this is passed
     *     along to create a temporary file, so it requires 6 trailing X
     *     characters.
     * @param script The contents of the temporary file, which will be executed
     *     when you call `run`.
     */
    SubProcess(const char* filenameTemplate, const char* script);

    /**
     * Destructor
     */
    ~SubProcess(void);

    /**
     * Retriev the process ID of the child process
     *
     * @return The child process's PID
     */
    [[nodiscard]] inline pid_t pid(void) const
    {
        return mPid;
    }

    /**
     * @return The return code of the child process (once it has completed), or
     *     -1 beforehand
     */
    [[nodiscard]] inline int returnCode(void) const
    {
        return mRc;
    }

    /**
     * Read the stdout from the child process and return it.
     *
     * @return The stdout
     */
    const std::string& stdout(void);

    /**
     * Read the stderr from the child process and return it.
     *
     * @return The stderr
     */
    const std::string& stderr(void);

    /**
     * Run the sub process
     *
     * @return -1 in the case of an error being encountered, otherwise the PID
     *     of the child process that was spawned.
     */
    int run(void);

    /**
     * Wait for the child process to complete
     *
     * @return The PID of the child process, or -1 on an error
     */
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
