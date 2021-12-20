#include "Logger.hpp"

std::auto_ptr<Logger> Logger::instance;

Logger::Logger() : init(false) {
	cLog.reset(new std::ostream(std::cerr.rdbuf()));
	log.reset(new std::ostream(std::cout.rdbuf()));
	dLog.reset(new std::ofstream("/dev/null"));
}

Logger& Logger::Get() {
	if(!instance.get())
		instance.reset(new Logger());

	return *instance;
}

void Logger::Init(const std::string CriticalLog, const std::string Log, const std::string DebugLog) {
	if(CriticalLog.length()) {
		cLog.reset(new std::ofstream(CriticalLog.c_str(), std::ofstream::app));
		cLogFile = CriticalLog;
		if(!(*cLog).good()) {
			std::cerr << "Failed to open log file '" << CriticalLog << "'." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	if(Log.length()) {
		log.reset(new std::ofstream(Log.c_str(), std::ofstream::app));
		logFile = Log;
		if(!(*log).good()) {
			std::cerr << "Failed to open log file '" << Log << "'." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	if(DebugLog.length()) {
		dLog.reset(new std::ofstream(DebugLog.c_str(), std::ofstream::app));
		dLogFile = DebugLog;
		if(!(*dLog).good()) {
			std::cerr << "Failed to open log file '" << DebugLog << "'." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	init = true;
}

std::ostream& Logger::GetCriticalLogStream() {
	if(!instance.get())
		instance.reset(new Logger());

	return *instance->cLog;
}

std::ostream& Logger::GetLogStream() {
	if(!instance.get())
		instance.reset(new Logger());

	return *instance->log;
}

std::ostream& Logger::GetDebugLogStream() {
	if(!instance.get())
		instance.reset(new Logger());

	return *instance->dLog;
}

void Logger::LogCritical(const std::string &Str) {
	*cLog << Str;
	if(!(*cLog).good()) {
		if(cLogFile.length())
			std::cerr << "Failed to write to  file '" << cLogFile << "': " << Str;
		exit(EXIT_FAILURE);
	}
}

void Logger::Log(const std::string &Str) {
	*log << Str;
	if(!(*log).good())
		if(logFile.length()) {
			std::stringstream mesg;
			mesg << "Failed to write to log file '" << logFile << "': " << Str;
			LogCritical(mesg.str());
		}
}

void Logger::LogDebug(const std::string &Str) {
	if(dLogFile.length())
		*dLog << Str;
		if(!(*dLog).good()) {
			std::stringstream mesg;
			mesg << "Failed to write to log file '" << dLogFile << "': " << Str;
			LogCritical(mesg.str());
		}
}

void Logger::Shutdown() {
	if(init) {
		if((*cLog).good()) {
			(*cLog).flush();
			cLog.reset(new std::ostream(std::cerr.rdbuf()));
		}

		if((*log).good()) {
			(*log).flush();
			log.reset(new std::ostream(std::cout.rdbuf()));
		}
		
		if((*dLog).good()) {
			(*dLog).flush();
			dLog.reset(new std::ofstream("/dev/null"));
		}
		
		init = false;
	}
}

Logger::~Logger() {
}
