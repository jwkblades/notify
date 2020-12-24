#include "TestUtilities.hpp"
#include "SubProcess.hpp"

TEST(SubProcessTemplateException, what)
{
    SubProcessTemplateException e;
    EXPECT_EQUAL(std::string("SubProcess filename template must have at least 6 'X' characters at the end of it."), e.what());
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

    p.run();
    p.wait();

    EXPECT_EQUAL(0, p.returnCode());
    EXPECT_NOT_EQUAL(-1, p.pid());
}

TEST(SubProcess, badReturnCode)
{
    SubProcess p("/tmp/subProcessTest-XXXXXX",
        R"__(#!/bin/bash

exit 1
)__");

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

    p.run();
    p.wait();

    EXPECT_EQUAL(0, p.returnCode());
    EXPECT_EQUAL(std::string("This is a test"), p.stdout());
    EXPECT_EQUAL(std::string("This is an error"), p.stderr());
}
