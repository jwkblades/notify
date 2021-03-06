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

#ifndef __NOTIFY_TEST_UTILITIES_H
#define __NOTIFY_TEST_UTILITIES_H

#include <chrono>
#include <iostream>

#include <gtest/gtest.h>

// Additional expect macros
#define EXPECT_NULL(item) EXPECT_TRUE(NULL == item)
#define EXPECT_NOT_NULL(item) EXPECT_FALSE(NULL == item)
#define EXPECT_EQUAL(a, b) EXPECT_EQ(a, b)
#define EXPECT_NOT_EQUAL(a, b) EXPECT_NE(a, b)

// Additional assert macros
#define ASSERT_NULL(item) ASSERT_TRUE(NULL == item)
#define ASSERT_NOT_NULL(item) ASSERT_FALSE(NULL == item)
#define ASSERT_EQUAL(a, b) ASSERT_EQ(a, b) << LVALS(a, b)
#define ASSERT_NOT_EQUAL(a, b) ASSERT_NE(a, b)


#endif
