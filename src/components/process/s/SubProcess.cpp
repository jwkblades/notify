#include "SubProcess.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

const char* SubProcessTemplateException::what(void) const noexcept
{
    return "SubProcess filename template must have at least 6 'X' characters at the end of it.";
}

SubProcess::SubProcess(const char* filename, const char* script):
    mPid(-1),
    mFilename(NULL),
    mScript(script),
    mStdout(),
    mStderr(),
    mRc(0)
{
    size_t len = strlen(filename);
    if (len < SUB_PROCESS_TEMPLATE_LENGTH)
    {
        throw SubProcessTemplateException();
    }

    for (size_t i = len - 6; i < len; ++i)
    {
        if (filename[i] != 'X')
        {
            throw SubProcessTemplateException();
        }
    }

    // NOTE: +1 here is for the '\0' character
    mFilename = new char[len + 1];
    memcpy(mFilename, filename, len + 1);
}

SubProcess::~SubProcess(void)
{
    if (mPid == -1)
    {
        delete [] mFilename;
        return;
    }

    if (mPid == 0) // child process
    {
        close(mStdout[1]);
        close(mStderr[1]);
        delete [] mFilename;
    }
    else
    {
        close(mStdout[0]);
        close(mStderr[0]);

        unlink(mFilename);
        delete [] mFilename;
    }
}

const std::string& SubProcess::stdout(void)
{
    char buf[512];
    ssize_t bytes = 0;
    while ((bytes = read(mStdout[0], buf, 512)) > 0)
    {
        mStdoutString += std::string(buf, bytes);
    }

    return mStdoutString;
}

const std::string& SubProcess::stderr(void)
{
    char buf[512];
    ssize_t bytes = 0;
    while ((bytes = read(mStderr[0], buf, 512)) > 0)
    {
        mStderrString += std::string(buf, bytes);
    }

    return mStderrString;
}

int SubProcess::run(void)
{
    int fd = mkostemp(mFilename, O_CLOEXEC | O_SYNC);
    if (fd == -1)
    {
        return -1;
    }

    size_t offset = 0;
    size_t total = strlen(mScript);
    ssize_t bytesWritten = 0;
    while (offset < total)
    {
        bytesWritten = write(fd, mScript + offset, total - offset);
        if (bytesWritten > 0)
        {
            offset += bytesWritten;
        }
        else
        {
            // In the case where we cannot write to the file, it means that we
            // have either had it close unexpectedly on us, an error has
            // happened, or we are for some reason litterally attempting to
            // write 0 bytes. These are all error cases.
            return -1;
        }
    }
    close(fd);
    if (chmod(mFilename, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
    {
        return -1;
    }

    pipe(mStdout);
    pipe(mStderr);

    mPid = vfork();

    if (mPid == -1)
    {
        return -1;
    }

    if (mPid == 0) // child process
    {
        close(mStdout[0]);
        close(mStderr[0]);

        dup2(mStdout[1], fileno(::stdout));
        dup2(mStderr[1], fileno(::stderr));

        char* args[] = {NULL};
        char* env[] = {NULL};
        return execve(mFilename, args, env);
    }
    else
    {
        close(mStdout[1]);
        close(mStderr[1]);
    }

    return mPid;
}

int SubProcess::wait(void)
{
    int status = 0;
    int ret = waitpid(mPid, &status, 0); // block, waiting for the process to return
    if (WIFEXITED(status))
    {
        mRc = WEXITSTATUS(status);
    }
    return ret;
}
