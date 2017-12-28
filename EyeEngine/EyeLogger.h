#pragma once
#include <log4cplus\logger.h>
#include <log4cplus\loggingmacros.h>

namespace EyeEngine
{

class EyeLogger
{
public:
	template<typename...FORMAT_ARGS>
	inline static void LogInfo(const std::wstring& fmt, FORMAT_ARGS...args)
	{
		std::wprintf(fmt, std::forward<FORMAT_ARGS>(args)...);
	}
};

}// namespace EyeEngine