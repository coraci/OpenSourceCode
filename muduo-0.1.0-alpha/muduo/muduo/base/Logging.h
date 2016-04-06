#ifndef MUDUO_LOG_LOGGING_H
#define MUDUO_LOG_LOGGING_H

#include <ostream>
#include <boost/scoped_ptr.hpp>

namespace muduo
{

// 日志记录实现
class LoggerImpl;

// 日志记录器
class Logger
{
 public:
 
 // 日志级别
  enum LogLevel
  {
    TRACE,		// 跟踪
    DEBUG,		// 调试
    INFO,		// 信息打印
    WARN,		// 警告
    ERROR,		// 错误
    FATAL,		// 致命错误
    NUM_LOG_LEVELS,
  };

  // 各种构造函数
  Logger(const char* file, int line);
  Logger(const char* file, int line, LogLevel level);
  Logger(const char* file, int line, LogLevel level, const char* func);
  Logger(const char* file, int line, bool toAbort);
  ~Logger();

  std::ostream& stream();

  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

 private:
  // 日志池
  boost::scoped_ptr<LoggerImpl> impl_;
};

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

// 检测指针是否为空
#define CHECK_NOTNULL(val) \
  ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(const char *file, int line, const char *names, T* ptr) {
  if (ptr == NULL) {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

}

#endif  // MUDUO_LOG_LOGGING_H
