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

const char* SubProcessTempFileException::what(void) const noexcept
{
    return "SubProcess was unable to set up the script file.";
}

SubProcess::SubProcess(const char* filename, const char* script):
    mPid(-1),
    mFilename(NULL),
    mStdout(),
    mStderr(),
    mRc(-1)
{
    setupFilename(filename);

    writeTempFileContents(script);
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
        return execve(mFilename, args, environ);
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

void SubProcess::setupFilename(const char* filename)
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

void SubProcess::writeTempFileContents(const char* script)
{
    int fd = mkostemp(mFilename, O_CLOEXEC | O_SYNC);
    if (fd == -1)
    {
        throw SubProcessTempFileException();
    }

    size_t offset = 0;
    size_t total = strlen(script);
    ssize_t bytesWritten = 0;
    while (offset < total)
    {
        bytesWritten = write(fd, script + offset, total - offset);
        // NOTE: Not being able to write any bytes is bad.
        if (bytesWritten > 0)
        {
            offset += bytesWritten;
        }
        else
        {
            throw SubProcessTempFileException();
        }
    }

    close(fd);
    if (chmod(mFilename, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
    {
        throw SubProcessTempFileException();
    }
}
