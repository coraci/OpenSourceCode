// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_TIMERID_H
#define MUDUO_NET_TIMERID_H

#include <muduo/base/copyable.h>

namespace muduo
{
namespace net
{

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
// 计时器的id？用于取消计时器
class TimerId : public muduo::copyable
{
 public:
  explicit TimerId(Timer* timer)
    : value_(timer)
  {
  }

 private:
  Timer* value_;
};

}
}

#endif  // MUDUO_NET_TIMERID_H
