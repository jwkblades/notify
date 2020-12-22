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
#include "Configuration.hpp"

struct kvPair
{
public:
    const char* key;
    const char* value;
};

/**
 * This test is solely meant to ensure that we can construct a mostly default
 * Configuration, and that it is invalid afterwards.
 */
TEST(Configuration, Base)
{
    Configuration c(0, NULL, "", NULL);
    EXPECT_FALSE(c.valid());
}

/**
 * Verify that we can create a basic valid configuration (in this case, with
 * just a title and description)
 */
TEST(Configuration, Basic)
{
    const char* args[] = {
        "Bogus application instance",
        "A title",
        "A simple description"
    };
    Configuration c(3, (char**)args, "", NULL);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);
}

/**
 * Unrecognized option
 */
TEST(Configuration, UnrecognizedOption)
{
    const char* args[] = {
        "Bogus application instance",
        "--waffleIron",
        "A title",
        "A simple description"
    };
    Configuration c(4, (char**)args, "", NULL);
    EXPECT_FALSE(c.valid());
}

/**
 * Now we are going to add some options in to the mix, this should result in a
 * valid final configuration.
 */
TEST(Configuration, OptionsMatched)
{
    kvPair option1 = {
        .key = "First",
        .value = "1"
    };
    const char* args[] = {
        "Bogus application instance",
        "-o",
        option1.key,
        "-v",
        option1.value,
        "A title",
        "A simple description"
    };
    struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration c(7, (char**)args, "i:o:t:v:", longOptions);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);

    EXPECT_EQUAL(1, c.optIndex);
    EXPECT_EQUAL(option1.key, c.options[0]);
    EXPECT_EQUAL(option1.value, c.values[0]);
}

/**
 * And now, add the maximum number of options to verify
 */
TEST(Configuration, OptionsMaxMatched)
{
    kvPair option1 = {
        .key = "First",
        .value = "1"
    };
    kvPair option2 = {
        .key = "Second",
        .value = "2"
    };
    kvPair option3 = {
        .key = "Third",
        .value = "3"
    };
    const char* args[] = {
        "Bogus application instance",
        "-o",
        option1.key,
        "-v",
        option1.value,
        "-o",
        option2.key,
        "-v",
        option2.value,
        "-o",
        option3.key,
        "-v",
        option3.value,
        "A title",
        "A simple description"
    };
    struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration c(15, (char**)args, "i:o:t:v:", longOptions);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);

    EXPECT_EQUAL(3, c.optIndex);
    EXPECT_EQUAL(CONFIGURATION_MAX_OPTIONS, c.optIndex);
    EXPECT_EQUAL(option1.key, c.options[0]);
    EXPECT_EQUAL(option1.value, c.values[0]);
    EXPECT_EQUAL(option2.key, c.options[1]);
    EXPECT_EQUAL(option2.value, c.values[1]);
    EXPECT_EQUAL(option3.key, c.options[2]);
    EXPECT_EQUAL(option3.value, c.values[2]);
}

/**
 * And now, add the maximum+1 number of options to verify
 */
TEST(Configuration, OptionsOverMaxMatched)
{
    kvPair option1 = {
        .key = "First",
        .value = "1"
    };
    kvPair option2 = {
        .key = "Second",
        .value = "2"
    };
    kvPair option3 = {
        .key = "Third",
        .value = "3"
    };
    kvPair option4 = {
        .key = "Fourth",
        .value = "4"
    };
    const char* args[] = {
        "Bogus application instance",
        "-o",
        option1.key,
        "-v",
        option1.value,
        "-o",
        option2.key,
        "-v",
        option2.value,
        "-o",
        option3.key,
        "-v",
        option3.value,
        "-o",
        option4.key,
        "-v",
        option4.value,
        "A title",
        "A simple description"
    };
    struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration c(19, (char**)args, "i:o:t:v:", longOptions);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);

    EXPECT_EQUAL(3, c.optIndex);
    EXPECT_EQUAL(CONFIGURATION_MAX_OPTIONS, c.optIndex);
    EXPECT_EQUAL(option1.key, c.options[0]);
    EXPECT_EQUAL(option1.value, c.values[0]);
    EXPECT_EQUAL(option2.key, c.options[1]);
    EXPECT_EQUAL(option2.value, c.values[1]);
    EXPECT_EQUAL(option3.key, c.options[2]);
    EXPECT_EQUAL(option3.value, c.values[2]);
}

/**
 * Mismatched options and values
 */
TEST(Configuration, OptionsMismatched)
{
    kvPair option1 = {
        .key = "First",
        .value = "1"
    };
    kvPair option2 = {
        .key = "Second",
        .value = "2"
    };
    kvPair option3 = {
        .key = "Third",
        .value = "3"
    };
    const char* args[] = {
        "Bogus application instance",
        "-o",
        option1.key,
        "-v",
        option1.value,
        "-o",
        option2.key,
        "-o",
        option3.key,
        "A title",
        "A simple description"
    };
    struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration c(11, (char**)args, "i:o:t:v:", longOptions);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);

    EXPECT_EQUAL(1, c.optIndex);
    EXPECT_EQUAL(option1.key, c.options[0]);
    EXPECT_EQUAL(option1.value, c.values[0]);
}

/**
 * Set icon, and timeout, add some long options to the mix
 */
TEST(Configuration, LongOptions)
{
    kvPair option1 = {
        .key = "First",
        .value = "1"
    };
    kvPair option2 = {
        .key = "Second",
        .value = "2"
    };
    kvPair option3 = {
        .key = "Third",
        .value = "3"
    };
    const char* timeout = "5";
    const char* icon = "example";
    const char* args[] = {
        "Bogus application instance",
        "--option",
        option1.key,
        "--value",
        option1.value,
        "--icon",
        icon,
        "-o",
        option2.key,
        "-v",
        option2.value,
        "-o",
        option3.key,
        "--timeout",
        timeout,
        "-v",
        option3.value,
        "A title",
        "A simple description"
    };
    struct option longOptions[] = {
        {"option",       required_argument, 0, 'o'},
        {"value",        required_argument, 0, 'v'},
        {"timeout",      required_argument, 0, 't'},
        {"icon",         required_argument, 0, 'i'}
    };

    Configuration c(19, (char**)args, "i:o:t:v:", longOptions);
    EXPECT_TRUE(c.valid());
    EXPECT_EQUAL("A title", c.title);
    EXPECT_EQUAL("A simple description", c.description);

    EXPECT_EQUAL(3, c.optIndex);
    EXPECT_EQUAL(CONFIGURATION_MAX_OPTIONS, c.optIndex);
    EXPECT_EQUAL(option1.key, c.options[0]);
    EXPECT_EQUAL(option1.value, c.values[0]);
    EXPECT_EQUAL(option2.key, c.options[1]);
    EXPECT_EQUAL(option2.value, c.values[1]);
    EXPECT_EQUAL(option3.key, c.options[2]);
    EXPECT_EQUAL(option3.value, c.values[2]);

    EXPECT_EQUAL(std::stoull(timeout), c.timeoutMinutes);
    EXPECT_EQUAL(icon, c.icon);
}
