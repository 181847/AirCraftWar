#pragma once
#include <log4cplus\logger.h>
#include <log4cplus\loggingmacros.h>

#define LOG_DEBUG(logMsg)	LOG4CPLUS_DEBUG(g_eyeLogger, errorMsg)
#define LOG_INFO(logMsg)	LOG4CPLUS_INFO(g_eyeLogger, errorMsg)
#define LOG_WARN(logMsg)	LOG4CPLUS_WARN(g_eyeLogger, errorMsg)
#define LOG_DEBUG(logMsg)	LOG4CPLUS_ERROR(g_eyeLogger, errorMsg)
#define LOG_FATAL(logMsg)	LOG4CPLUS_FATAL(g_eyeLogger, errorMsg)

namespace EyeEngine
{
namespace EyeLogger
{

extern log4cplus::Logger g_eyeLogger;

// Init the global logger for EyeEngine.
void InitLogger();

}// namespace EyeLogger

}// namespace EyeEngine