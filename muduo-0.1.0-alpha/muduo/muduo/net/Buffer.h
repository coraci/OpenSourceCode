// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_BUFFER_H
#define MUDUO_NET_BUFFER_H

#include <muduo/base/copyable.h>
#include <muduo/base/Types.h>

#include <algorithm>
#include <vector>

#include <assert.h>
//#include <unistd.h>  // ssize_t

namespace muduo
{
namespace net
{

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
// 缓冲区
class Buffer : public muduo::copyable
{
 public:
  // 便宜的预先考虑的缓冲区大小
  static const size_t kCheapPrepend = 8;
  // 初始化的缓冲区大小
  static const size_t kInitialSize = 1024;

  Buffer()
    : buffer_(kCheapPrepend + kInitialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
  {
    assert(readableBytes() == 0);
    assert(writableBytes() == kInitialSize);
    assert(prependableBytes() == kCheapPrepend);
  }

  // default copy-ctor, dtor and assignment are fine

  // 缓冲区交换
  void swap(Buffer& rhs)
  {
    buffer_.swap(rhs.buffer_);
    std::swap(readerIndex_, rhs.readerIndex_);
    std::swap(writerIndex_, rhs.writerIndex_);
  }

  // 可读的字节的数量
  size_t readableBytes()
  { return writerIndex_ - readerIndex_; }

  // 写空间的剩余字节数
  size_t writableBytes()
  { return buffer_.size() - writerIndex_; }

  // 预先考虑的字节的数量
  // 实际返回的是读索引
  size_t prependableBytes()
  { return readerIndex_; }

  // 取出字符串的其实位置
  const char* peek() const
  { return begin() + readerIndex_; }

  // 查找\r\n
  const char* findCRLF() const
  {
    const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
    return crlf == beginWrite() ? NULL : crlf;
  }

  const char* findCRLF(const char* start) const
  {
    assert(peek() <= start);
    assert(start <= beginWrite());
    const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
    return crlf == beginWrite() ? NULL : crlf;
  }

  // retrieve returns void, to prevent
  // string str(retrieve(readableBytes()), readableBytes());
  // the evaluation of two functions are unspecified
  // 重新检索，从指定位置开始
  void retrieve(size_t len)
  {
    assert(len <= readableBytes());
    readerIndex_ += len;
  }

  void retrieveUntil(const char* end)
  {
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end - peek());
  }

  void retrieveAll()
  {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }

  string retrieveAsString()
  {
    string str(peek(), readableBytes());
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
    return str;
  }

  void append(const string& str)
  {
    append(str.data(), str.length());
  }

  void append(const char* /*restrict*/ data, size_t len)
  {
    if (writableBytes() < len)
    {
      makeSpace(len);
    }
    assert(len <= writableBytes());
    std::copy(data, data+len, beginWrite());
    writerIndex_ += len;
  }

  void prepend(const void* /*restrict*/ data, size_t len)
  {
    assert(len <= prependableBytes());
    readerIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, begin()+readerIndex_);
  }

  void shrink(size_t reserve)
  {
   std::vector<char> buf(kCheapPrepend+readableBytes()+reserve);
   std::copy(peek(), peek()+readableBytes(), buf.begin()+kCheapPrepend);
   buf.swap(buffer_);
  }

  /// Read data directly into buffer.
  ///
  /// It may implement with readv(2)
  /// @return result of read(2), @c errno is saved
  ssize_t readFd(int fd, int* savedErrno);

 private:
   char* beginWrite()
   { return begin() + writerIndex_; }

   const char* beginWrite() const
   { return begin() + writerIndex_; }

   char* begin()
   { return &*buffer_.begin(); }

   const char* begin() const
   { return &*buffer_.begin(); }

   void makeSpace(size_t more)
   {
     if (writableBytes() + prependableBytes() < more + kCheapPrepend)
     {
       buffer_.resize(writerIndex_+more);
     }
     else
     {
       // move readable data to the front, make space inside buffer
       size_t used = readableBytes();
       std::copy(begin()+readerIndex_,
                 begin()+writerIndex_,
                 begin()+kCheapPrepend);
       readerIndex_ = kCheapPrepend;
       writerIndex_ = readerIndex_ + used;
       assert(used == readableBytes());
     }
   }

 private:
   std::vector<char> buffer_;
   size_t readerIndex_;
   size_t writerIndex_;
   static const char kCRLF[];
};

}
}

#endif  // MUDUO_NET_BUFFER_H
