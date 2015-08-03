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

#include "SaferFD.h"
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>


using namespace std;

namespace saferfd {

const int UNSET_FD = -1;

FD::FD() :
    mFd(UNSET_FD)
{
    // unset state
}

FD::FD(int& fd)
{
    mFd = fd;
    if(! *this)
        throw runtime_error("fd provided is invalid");

    // here we take ownership and attempt to prevent caller from reusing variable
    // maybe we should just dup
    fd = UNSET_FD;
}

FD::FD(const FD& other)
{
    if(-1 == (mFd = dup(other.mFd)))
        throw runtime_error("SafeFD::copy other SafeFD is invalid");
}

FD::FD(FD&& other)
{
    if(other)
    {
        reset();
        std::swap(other.mFd, mFd);
    }
    else
        throw runtime_error("SafeFD::move other SafeFD is invalid");
}

FD& FD::operator=(const FD& other)
{
    // handle case of s = s
    if(this != &other)
    {
        // assiging with strong guarantee
        int fd;
        if(-1 != (fd = dup(other.mFd)))
        {
            reset();
            mFd = fd;
        }
        else
            throw runtime_error("other SafeFD is invalid");
    }

    return *this;
}

FD &FD::operator=(FD&& other)
{
    // handle case of s = s
    if(this != &other)
    {
        if(other)
        {
            reset();
            std::swap(other.mFd, mFd);
        }
        else
            throw runtime_error("SafeFD::move other SafeFD is invalid");
    }

    return *this;
}

FD::operator bool() const
{
    return (UNSET_FD != mFd) && (UNSET_FD != fcntl(mFd, F_GETFD));
}

FD& FD::operator=(int& fd)
{
    FD tmp(fd);
    *this = move(tmp);
    return *this;
}

void FD::reset()
{
    if(UNSET_FD != mFd)
    {
        close(mFd);
        mFd = UNSET_FD;
    }
}

FD::~FD()
{
    reset();
}

int FD::operator*() const
{
    if(! *this)
        throw runtime_error("SaferFD is unset");

    return mFd;
}


FD open(const std::string& pathname, const int flags, const mode_t mode)
{
    try
    {
        int fd = ::open(pathname.c_str(), flags, mode);
        return FD(fd);
    }
    catch(const runtime_error&)
    {
        throw runtime_error(string("SaferFD::Unable to open file: ") + pathname);
    }
}

std::pair<FD, FD> socketpair(int domain, int type, int protocol)
{
    int fds[2];
    if(! ::socketpair(domain, type, protocol, fds))
        return pair<FD, FD>(FD(fds[0]), FD(fds[1]));
    else
        throw runtime_error(string("SaferFD::Socketpair"));
}

} // namespace
