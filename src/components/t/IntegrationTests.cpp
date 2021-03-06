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
#include "TestUtilities.hpp"

TEST(Integration, Script)
{
    SubProcess p("/tmp/notify-integration-XXXXXX", R"__(#!/bin/bash
./bin/intgtest
)__");
    p.run();
    p.wait();
    std::cerr << p.stderr() << std::endl;
    std::cout << p.stdout() << std::endl;
    EXPECT_EQUAL(0, p.returnCode());
}
