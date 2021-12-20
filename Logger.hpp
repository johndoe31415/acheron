#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define LOGGER Logger::Get().GetLogStream()
#define LOGGER_DEBUG Logger::Get().GetDebugLogStream()
#define LOGGER_CRIT Logger::Get().GetCriticalLogStream()

class Logger {
	private:
		Logger();

		static std::auto_ptr<Logger> instance;
		std::auto_ptr<std::ostream> cLog;
		std::auto_ptr<std::ostream> log;
		std::auto_ptr<std::ostream> dLog;
		std::string cLogFile;
		std::string logFile;
		std::string dLogFile;
		bool init;
	public:
		static Logger& Get();
		std::ostream& GetCriticalLogStream();
		std::ostream& GetLogStream();
		std::ostream& GetDebugLogStream();
		void Init(const std::string CriticalLog, const std::string Log, const std::string DebugLog);
		void LogCritical(const std::string &Str);
		void Log(const std::string &Str);
		void LogDebug(const std::string &Str);
		void Shutdown();
		~Logger();
};

#endif
