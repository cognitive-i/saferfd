/*
 * Copyright (c) 2015, Cognitive-i Ltd (saferfd@cognitive-i.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *    list of conditions and the following disclaimer in the documentation and/or
 *    other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SaferFD.h"
#include <fcntl.h>

using namespace std;
using namespace testing;
using namespace saferfd;

// SaferFD bool operator is explicit to cannot coerce with ASSERT_TRUE and EXPECT_TRUE
#define EXPECT_BOOL_TRUE(condition) \
    GTEST_TEST_BOOLEAN_(!!(condition), #condition, false, true, \
    GTEST_NONFATAL_FAILURE_)

#define ASSERT_BOOL_TRUE(condition) \
    GTEST_TEST_BOOLEAN_(!!(condition), #condition, false, true, \
    GTEST_FATAL_FAILURE_)


TEST(TestSaferFdConstruction, OpenHappyPath) {
    FD f = saferfd::open(__FILE__, O_RDONLY, 0);
    ASSERT_BOOL_TRUE(f);
}

TEST(TestSaferFdConstruction, CreateFromValidFD) {

    int fd = ::open(__FILE__, O_RDONLY, 0);
    ASSERT_NE(-1, fd);

    FD f(fd);
    EXPECT_BOOL_TRUE(f);
    EXPECT_EQ(-1, fd);
}

TEST(TestSaferFdConstruction, CreateFromInvalidFD) {

    int fd = -5;

    ASSERT_THROW({
                     FD f(fd);
                 },
                 runtime_error);

    EXPECT_EQ(-5, fd);
}

TEST(TestSaferFdConstruction, DerefInvalid) {
    FD s;
    EXPECT_FALSE(s);
    ASSERT_THROW({
                     int d = *s;
                 },
                 runtime_error);

    ASSERT_THROW({
                     FD d(s);
                 },
                 runtime_error);

}

TEST(TestTransferOwnership, CopyConstruction) {

    FD s = saferfd::open(__FILE__, O_RDONLY, 0);
    FD d(s);

    EXPECT_BOOL_TRUE(s);
    EXPECT_BOOL_TRUE(d);
    EXPECT_NE(*s, *d);
}

TEST(TestTransferOwnership, SimpleAssignment) {

    FD d;
    ASSERT_FALSE(d);

    FD s = saferfd::open(__FILE__, O_RDONLY, 0);
    d = s;
    EXPECT_BOOL_TRUE(d);
    EXPECT_BOOL_TRUE(s);
}

TEST(TestSaferFdConstruction, MoveAssignment) {
    FD s = saferfd::open(__FILE__, O_RDONLY, 0);
    int sPrevious = *s;
    FD d = std::move(s);

    EXPECT_BOOL_TRUE(d);
    EXPECT_FALSE(s);
    EXPECT_EQ(sPrevious, *d);
}

TEST(TestSaferFdConstruction, MoveConstruction) {
    FD s = saferfd::open(__FILE__, O_RDONLY, 0);
    int sPrevious = *s;
    FD d(std::move(s));

    EXPECT_BOOL_TRUE(d);
    EXPECT_FALSE(s);
    EXPECT_EQ(sPrevious, *d);
}

TEST(TestTransferOwnership, SelfAssignmentShouldBeNOP) {
    FD s = saferfd::open(__FILE__, O_RDONLY, 0);
    int sPrevious = *s;
    s = s;
    EXPECT_BOOL_TRUE(s);
    EXPECT_EQ(*s, sPrevious);
}

TEST(TestTransferOwnership, AssignmentFromFd) {
    FD d;
    ASSERT_FALSE(d);

    int fd = ::open(__FILE__, O_RDONLY, 0);
    int backup = fd;

    ASSERT_NE(-1, fd);
    d = fd;

    // takes ownership of fd and -1 sets fd
    EXPECT_EQ(-1, fd);
    EXPECT_BOOL_TRUE(d);

    // check this actually points to the FD, we don't expect a dupe in this path
    EXPECT_EQ(backup, *d);
}





