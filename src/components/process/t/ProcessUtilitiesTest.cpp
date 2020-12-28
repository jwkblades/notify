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
#include "ProcessUtilities.hpp"
#include "SubProcess.hpp"

TEST(ProcessUtilities, sanitizeForBash)
{
    auto helper = [](const std::string& scriptString) -> void
    {
        std::string script = "#!/bin/bash\necho -n \"" + sanitizeForBash(scriptString.c_str()) + "\"";
        SubProcess p("/tmp/process-utilities-test-XXXXXX", script.c_str());
        p.run();
        p.wait();

        EXPECT_EQUAL(0, p.returnCode());
        EXPECT_EQUAL(scriptString, p.stdout());
    };


    for (const std::string& s : {
            "test\"; exit 1",
            "test\\\"; exit 1"
            "test \" && exit 1",
            "test $(exit 1)",
            "$(pwd)",
            "1.2.3\" | cut -d. -f1",
            "test `pwd`",
            "'test \" && true || false'"
        })
    {
        helper(s);
    }
}
