/*
 main.c     UNIT tests main for HSTR shell history completion utility

 Copyright (C) 2014-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/


#include "../../src/include/hstr.h"

// HSTR uses Unity C test framework: https://github.com/ThrowTheSwitch/Unity
#include "unity/src/c/unity.h"

/*
 * IMPORTANT: make sure to run TEST RUNNER GENERATOR script after any change!
 *
 */

void setUp(void)
{
}

void tearDown(void)
{
}

void test_failure(void)
{
    TEST_ASSERT_TRUE(1);
    TEST_ASSERT_FALSE(1);
}

void test_help(void)
{
    //return hstrMain(argc, argv);
}
