#pragma once
#include <log4cplus\logger.h>
#include <log4cplus\loggingmacros.h>

#define LOG_DEBUG(logMsg)	LOG4CPLUS_DEBUG(EyeEngine::EyeLogger::g_eyeLogger, logMsg)
#define LOG_INFO(logMsg)	LOG4CPLUS_INFO(EyeEngine::EyeLogger::g_eyeLogger, logMsg)
#define LOG_WARN(logMsg)	LOG4CPLUS_WARN(EyeEngine::EyeLogger::g_eyeLogger, logMsg)
#define LOG_DEBUG(logMsg)	LOG4CPLUS_ERROR(EyeEngine::EyeLogger::g_eyeLogger, logMsg)
#define LOG_FATAL(logMsg)	LOG4CPLUS_FATAL(EyeEngine::EyeLogger::g_eyeLogger, logMsg)

namespace EyeEngine
{
namespace EyeLogger
{

extern log4cplus::Logger g_eyeLogger;

// Init the global logger for EyeEngine.
void InitLogger();

}// namespace EyeLogger

}// namespace EyeEngine