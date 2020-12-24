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

#include "TestUtilities.hpp"
#include "SubProcess.hpp"

TEST(SubProcessTemplateException, what)
{
    SubProcessTemplateException e;
    EXPECT_EQUAL(std::string("SubProcess filename template must have at least 6 'X' characters at the end of it."), e.what());
}

TEST(SubProcessTempFileException, what)
{
    SubProcessTempFileException e;
    EXPECT_EQUAL(std::string("SubProcess was unable to set up the script file."), e.what());
}

TEST(SubProcess, filenameTooShort)
{
    EXPECT_THROW(SubProcess p("/a", ""), SubProcessTemplateException);
}

TEST(SubProcess, badFilenameTemplate)
{
    EXPECT_THROW(SubProcess p("/tmp/waffles-XFSER", ""), SubProcessTemplateException);
}

TEST(SubProcess, simple)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

sleep 1
)__");

    EXPECT_EQUAL(-1, p.returnCode());

    p.run();
    p.wait();

    EXPECT_EQUAL(0, p.returnCode());
    EXPECT_NOT_EQUAL(-1, p.pid());
}

TEST(SubProcess, noRun)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

sleep 1
)__");

    EXPECT_EQUAL(-1, p.returnCode());
    EXPECT_EQUAL(-1, p.pid());
}

TEST(SubProcess, badReturnCode)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

exit 1
)__");

    EXPECT_EQUAL(-1, p.returnCode());

    p.run();
    p.wait();

    EXPECT_EQUAL(1, p.returnCode());
    EXPECT_NOT_EQUAL(-1, p.pid());
}

TEST(SubProcess, badReturnCode42)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

exit 42
)__");

    EXPECT_EQUAL(-1, p.returnCode());

    p.run();
    p.wait();

    EXPECT_EQUAL(42, p.returnCode());
    EXPECT_NOT_EQUAL(-1, p.pid());
}

TEST(SubProcess, readOutput)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

echo -n "This is a test"
echo -n "This is an error" >&2
)__");

    EXPECT_EQUAL(-1, p.returnCode());

    p.run();
    p.wait();

    EXPECT_EQUAL(0, p.returnCode());
    EXPECT_EQUAL(std::string("This is a test"), p.stdout());
    EXPECT_EQUAL(std::string("This is an error"), p.stderr());
}
