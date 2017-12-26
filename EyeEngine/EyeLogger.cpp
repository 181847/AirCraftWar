#include "EyeLogger.h"
#include <log4cplus\configurator.h>
#include <log4cplus\consoleappender.h>
#include <log4cplus\layout.h>

#pragma comment(lib, "log4cplusD.lib")

namespace EyeEngine
{
namespace EyeLogger
{

log4cplus::Logger g_eyeLogger;

void InitLogger()
{
	using log4cplus::Logger;
	using log4cplus::helpers::SharedObjectPtr;
	using log4cplus::Appender;
	using log4cplus::ConsoleAppender;
	using log4cplus::PatternLayout;


	SharedObjectPtr<Appender> consoleAppender(new ConsoleAppender());
	consoleAppender->setName(LOG4CPLUS_TEXT("Global ConsoleAppender for EyeEngine"));
	// Log as this formate:
	// example start:
	// 12/25/17 12:59:28 -This is the first log message...[d:\visual studio 2015\projects\learningtests\trylog4cplus\trylog4cplus\trylog4cplus.cpp:42]
	// example end
	consoleAppender->setLayout(std::make_unique<PatternLayout>(LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S} %m[%l]%n")));

	g_eyeLogger = Logger::getInstance(LOG4CPLUS_TEXT("Global Logger for EyeEngine"));

	g_eyeLogger.addAppender(consoleAppender);
	g_eyeLogger.setLogLevel(log4cplus::ALL_LOG_LEVEL);

	// try to log with all the level
	LOG4CPLUS_DEBUG(g_eyeLogger, "testing log Debug");
	LOG4CPLUS_INFO(g_eyeLogger, "testing log Info");
	LOG4CPLUS_WARN(g_eyeLogger, "testing log Warn");
	LOG4CPLUS_ERROR(g_eyeLogger, "testing log Error");
	LOG4CPLUS_FATAL(g_eyeLogger, "testing log Fatal");
}

}// namespace EyeLogger

}// namespace EyeEngine